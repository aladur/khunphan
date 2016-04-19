/*
    KhunPhan.cpp

    Automatic solution finder for KhunPhan game
    Copyright (C) 2001-2016  W. Schwotzer

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "stdafx.h"
#include <ostream>
#include <stdexcept>
#include "KhunPhan.h"
#include "kpsdl12userinterface.h"
#include "kpsdl2userinterface.h"
#include "kpglutuserinterface.h"
#include "kpnodes.h"
#include "kpsolutionscountfunction.h"
#include "blogger.h"


/////////////////////////////////////////////////////////////////////
// Class KhunPhanApp
/////////////////////////////////////////////////////////////////////

KhunPhanApp::KhunPhanApp() :
    argc(0), argv(nullptr), canRun(false)
{
}

KhunPhanApp::KhunPhanApp(int p_argc, char **p_argv) :
    argc(p_argc), argv(p_argv), canRun(true)
{
    Initialize();
}

KhunPhanApp::~KhunPhanApp()
{
    if (pUserInterface)
    {
        pUserInterface->Close();
    }
}

void KhunPhanApp::InitializeSolutionTree()
{
    // Get non-owning pointer to state context.
    KPstateContext *pContext =
        dynamic_cast<KPstateContext *>(pUserInterface.get());
    KPsolutionsCountFunction fct;

    pContext->GetNodes().CalculateSolveCount();

    auto positions = pContext->GetNodes().GetSize();
    pContext->GetNodes().Iterate(fct);
    auto solutionsCount = fct.GetCount();

    BLogger::Log("Total positions found: ", positions);
    BLogger::Log("Total solutions found: ", solutionsCount);
    BLogger::Log(std::fixed, std::setprecision(2),
                 "Time to calculate all game positions: ",
                 pContext->GetNodes().GetCreateSolveTreeTime() / 1000.0, " ms");
    BLogger::Log(std::fixed, std::setprecision(2),
                 "Time to calculate solve count for all positions: ",
                 pContext->GetNodes().GetCalculateSolveCountTime() / 1000.0,
                 " ms");
}

bool KhunPhanApp::Initialize()
{
    BLogger::Log(PACKAGE " V" VERSION);
    BLogger::Log("Copyright (C) 2002-2016 Wolfgang Schwotzer");
    BLogger::Log("This is free software; see the source for copying "
                 "conditions");
    BLogger::Log("There is NO warranty; not even for MERCHANTABILITY or "
                 "FITNESS");
    BLogger::Log("FOR A PARTICULAR PURPOSE.");

    config.SetDefaultValues();
    config.ReadCommandLineParams(argc, argv);
    config.ReadFromFile();

    if (config.DisplayVersionOnly)
    {
        canRun = false;
        return false;
    }

    config.DebugPrint();

    // initialize all the tokens
    rootBoard.InitializeToken(TK_GREEN1, 1, 3);
    rootBoard.InitializeToken(TK_GREEN2, 2, 3);
    rootBoard.InitializeToken(TK_GREEN3, 1, 4);
    rootBoard.InitializeToken(TK_GREEN4, 2, 4);
    rootBoard.InitializeToken(TK_WHITE1, 0, 0);
    rootBoard.InitializeToken(TK_WHITE2, 3, 0);
    rootBoard.InitializeToken(TK_WHITE3, 0, 3);
    rootBoard.InitializeToken(TK_WHITE4, 3, 3);
    rootBoard.InitializeToken(TK_WHITE5, 1, 2);
    rootBoard.InitializeToken(TK_RED1,   1, 0);

    KPnode rootNode(rootBoard);
    KPUIBase *pUIBase = nullptr;

    switch (config.UserInterface)
    {
#ifdef HAVE_SDL2

        case 0:
            pUIBase = new KPSdl2UserInterface(rootNode, config);
            break;
#else
#ifdef HAVE_SDL

        case 0:
            pUIBase = new KPSdl12UserInterface(rootNode, config);
            break;
#endif
#endif

#if defined(HAVE_LIBGLUT) || defined(HAVE_LIBOPENGLUT)

        case 1:
            pUIBase = new KPGlutUserInterface(rootNode, config);
            break;
#endif

        default:
            throw std::runtime_error("No user interface initialized");
    }

    pUserInterface.reset(pUIBase);
    InitializeSolutionTree();

    return true;
}

void KhunPhanApp::Run()
{
    if (argv != nullptr && canRun)
    {
        pUserInterface->OpenWindow(argc, argv);
        pUserInterface->MainLoop();
    }
}

