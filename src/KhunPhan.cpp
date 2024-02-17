/*
    KhunPhan.cpp

    Automatic solution finder for KhunPhan game
    Copyright (C) 2001-2024  W. Schwotzer

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
#include "KhunPhan.h"
#include "kpnodes.h"
#include "kpuserinterfacefactory.h"
#include "blogger.h"


/////////////////////////////////////////////////////////////////////
// Class KhunPhanApp
/////////////////////////////////////////////////////////////////////

KhunPhanApp::KhunPhanApp(int p_argc, char **p_argv) :
    argc(p_argc), argv(p_argv), canRun(true)
{
    Initialize();
}

KhunPhanApp::~KhunPhanApp()
{
    if (userInterfacePtr)
    {
        userInterfacePtr->Close();
    }
}

void KhunPhanApp::InitializeSolutionTree()
{
    // Get non-owning pointer to state context.
    KPstateContext *pContext =
        dynamic_cast<KPstateContext *>(userInterfacePtr.get());

    pContext->GetNodes().CalculateSolveCount();

    auto positions = pContext->GetNodes().GetSize();
    auto solutionsCount = pContext->GetNodes().GetSolutionsCount();

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
    BLogger::Log("Copyright (C) 2002-2024 Wolfgang Schwotzer");
    BLogger::Log("This is free software; see the source for copying "
                 "conditions");
    BLogger::Log("There is NO warranty; not even for MERCHANTABILITY or "
                 "FITNESS");
    BLogger::Log("FOR A PARTICULAR PURPOSE.");

    config = std::make_shared<KPConfig>();

    config->SetDefaultValues();
    config->ReadCommandLineParams(argc, argv);
    config->ReadFromFile();

    if (config->DisplayVersionOnly)
    {
        canRun = false;
        return false;
    }

    config->DebugPrint();

    // Get board with all tokens in start position
    KPnode rootNode(KPboard::CreateRootBoard());

    userInterfacePtr = KPuserInterfaceFactory::Create(
                           config->UserInterface,
                           rootNode,
                           config);

    InitializeSolutionTree();

    return true;
}

void KhunPhanApp::Run()
{
    if (argv != nullptr && canRun)
    {
        // Application can run only once
        canRun = false;
        userInterfacePtr->OpenWindow(argc, argv);
        userInterfacePtr->MainLoop();
    }
}

