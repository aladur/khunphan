/*
    KhunPhan.cpp

    Automatic solution finder for KhunPhan game
    Copyright (C) 2001,2002,2003  W. Schwotzer

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

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <stdio.h>
#include "KhunPhan.h"
#include "kpnode.h"
#include "kpconfig.h"
#include "bthread.h"
#include "kpsdluserinterface.h"
#include "kpglutuserinterface.h"


KhunPhanApp *KhunPhanApp::instance = NULL;

/////////////////////////////////////////////////////////////////////
// Class KhunPhanApp
/////////////////////////////////////////////////////////////////////

KhunPhanApp::KhunPhanApp() : proot(NULL), userInterface(NULL)
{
}
  
KhunPhanApp::~KhunPhanApp()
{
  delete userInterface;
  userInterface = NULL;
  proot    = NULL;
  instance = NULL;
}

KhunPhanApp &KhunPhanApp::Instance()
{
  if (instance == NULL)
  {
    instance = new KhunPhanApp();
    atexit ( KhunPhanApp::finalize );
  }
  return *instance;
}
  
void KhunPhanApp::InitializeSolutionTree() {
  proot = new KPnode;

  // initialize all the tokens
  proot->InitializeToken(TK_GREEN1, "green", 1, 3, 1, 1);
  proot->InitializeToken(TK_GREEN2, "green", 2, 3, 1, 1);
  proot->InitializeToken(TK_GREEN3, "green", 1, 4, 1, 1);
  proot->InitializeToken(TK_GREEN4, "green", 2, 4, 1, 1);
  proot->InitializeToken(TK_WHITE1, "white", 0, 0, 1, 2);
  proot->InitializeToken(TK_WHITE2, "white", 3, 0, 1, 2);
  proot->InitializeToken(TK_WHITE3, "white", 0, 3, 1, 2);
  proot->InitializeToken(TK_WHITE4, "white", 3, 3, 1, 2);
  proot->InitializeToken(TK_WHITE5, "white", 1, 2, 2, 1);
  proot->InitializeToken(TK_RED1,   "red",   1, 0, 2, 2);

  KPnode::CreateSolveTree(*proot);
  // The solve count will be calculated in a separate thread
  // because it takes some time (~16 sec with AMD Athlon 900 MHz)
  // there is a static function KPnode::IsSolveCountAvailable()
  // which can be check for the result available
  if (!thread.Start())
  {
    // if starting Thread fails start the procedure sequentially 
    // and inform user
    message(mtWarning, " Could not start Thread. Please wait...\n");
    thread.Run();
  }

  // several checks:
  //KPboard::idHash.Check(stdout);
  //KPboard::idHash.fprintf(stdout);
  DEBUGPRINT1("Total positions found: %d\n", KPnode::LLSetFirstToRoot());
  DEBUGPRINT1("Total solutions found: %d\n", KPnode::GetSolutionsCount());
}

bool KhunPhanApp::Initialize(int argc, char **argv)
{
  appName = argv[0];
  DEBUGPRINT(PACKAGE " V" VERSION "\n");
  DEBUGPRINT("Copyright (C) 2002,2003 Wolfgang Schwotzer\n");
  DEBUGPRINT("This is free software; see the source for copying conditions.  There is NO\n");
  DEBUGPRINT("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
  for (int i = 1; i < argc; i++)
    if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0)
    {
      return false;
    }
  KPConfig::Instance().SetDefaultValues();
  KPConfig::Instance().ReadCommandLineParams(argc, argv);
  KPConfig::Instance().ReadFromFile();
  KPConfig::Instance().DebugPrint();

  return true;
}

bool KhunPhanApp::Run(int argc, char **argv)
{
  switch (KPConfig::Instance().UserInterface)
  {
    case 0: userInterface = new KPSdlUserInterface();  break;
    case 1: userInterface = new KPGlutUserInterface(); break;
    default: userInterface = NULL;
  }
  
  if ( userInterface == NULL || !userInterface->OpenWindow(argc, argv) )
    return false;

  userInterface->UpdateDataModel(proot);
  userInterface->MainLoop();

  return true;
}

void KhunPhanApp::Shutdown()
{
  if (userInterface->IsInitialized()) {
    userInterface->Close();
  }
  delete this;
}

