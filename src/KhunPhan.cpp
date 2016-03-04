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
#include "kpconfig.h"
#include "kpsdl12userinterface.h"
#include "kpsdl2userinterface.h"
#include "kpglutuserinterface.h"
#include "kpnodes.h"
#include "kpsolutionscountfunction.h"


/////////////////////////////////////////////////////////////////////
// Class KhunPhanApp
/////////////////////////////////////////////////////////////////////

KhunPhanApp::KhunPhanApp() : userInterface(NULL)
{
}

KhunPhanApp::~KhunPhanApp()
{
    delete userInterface;
    userInterface = NULL;
}

void KhunPhanApp::InitializeSolutionTree()
{
    KPstateContext *pContext = userInterface;
    KPsolutionsCountFunction fct;

    pContext->GetNodes().CalculateSolveCount();

    int positions = pContext->GetNodes().GetSize();
    pContext->GetNodes().Iterate(fct);
    int solutionsCount = fct.GetCount();

    LOG2("Total positions found: ", positions);
    LOG2("Total solutions found: ", solutionsCount);
    LOG5(std::fixed, std::setprecision(2),
         "Time to calculate all game positions: ",
         pContext->GetNodes().GetCreateSolveTreeTime() / 1000.0, " ms");
    LOG5(std::fixed, std::setprecision(2),
         "Time to calculate solve count for all positions: ",
         pContext->GetNodes().GetCalculateSolveCountTime() / 1000.0, " ms");
}

bool KhunPhanApp::Initialize(int argc, char **argv)
{
    KPConfig &config = KPConfig::Instance();

    LOG1(PACKAGE " V" VERSION);
    LOG1("Copyright (C) 2002-2016 Wolfgang Schwotzer");
    LOG1("This is free software; see the source for copying conditions");
    LOG1("There is NO warranty; not even for MERCHANTABILITY or FITNESS");
    LOG1("FOR A PARTICULAR PURPOSE.");

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0)
        {
            return false;
        }
    }

    config.SetDefaultValues();
    config.ReadCommandLineParams(argc, argv);
    config.ReadFromFile();
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

    switch (KPConfig::Instance().UserInterface)
    {
#ifdef HAVE_SDL2
        case 0:
            userInterface = new KPSdl2UserInterface(rootNode);
            break;
#else
#ifdef HAVE_SDL
        case 0:
            userInterface = new KPSdl12UserInterface(rootNode);
            break;
#endif
#endif

#if defined(HAVE_LIBGLUT) || defined(HAVE_LIBOPENGLUT)
        case 1:
            userInterface = new KPGlutUserInterface(rootNode);
            break;
#endif
        default:
            throw std::runtime_error("No user interface initialized");
    }

    InitializeSolutionTree();

    return true;
}

void KhunPhanApp::Run(int argc, char **argv)
{
    userInterface->OpenWindow(argc, argv);
    userInterface->MainLoop();
}

void KhunPhanApp::Shutdown()
{
    if (userInterface->IsInitialized())
    {
        userInterface->Close();
    }
    delete this;
}

