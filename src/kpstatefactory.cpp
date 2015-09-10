/*
    kpstatefactory.cpp

    Automatic solution finder for KhunPhan game
    Copyright (C) 2001-2015  W. Schwotzer

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

#include <stdio.h>

#include "misc1.h"
#include "kpstatefactory.h"
#include "kpstate.h"

#include "kpstatestartup.h"
#include "kpstatemeinmenu.h"
#include "kpstatefinish.h"
#include "kpstatesettings.h"
#include "kpstatecontrolsettings.h"
#include "kpstategraphicsettings.h"
#include "kpstategraphichint.h"
#include "kpstateaudiosettings.h"
#include "kpstatekeyboardhelp.h"
#include "kpstategame.h"
#include "kpstategamesolved.h"
#include "kpstateselectlanguage.h"
#include "kpstatetutorial1.h"
#include "kpstatetutorial2.h"
#include "kpstatetutorial3.h"
#include "kpstatetutorial4.h"
#include "kpstatescorelist.h"
#include "kpstatelighttest.h"


KPstateFactory *KPstateFactory::instance = NULL;

/////////////////////////////////////////////////////////////////////
// Public Interface
/////////////////////////////////////////////////////////////////////

KPstateFactory &KPstateFactory::Instance()
{
    if (instance == NULL)
    {
        instance = new KPstateFactory();
        atexit ( KPstateFactory::finalize );
    }
    return *instance;
}

KPstate *KPstateFactory::CreateState(int stateID)
{
    switch (stateID)
    {
        case KPState_StartUp:
            return new KPstateStartUp;
        case KPState_MainMenu:
            return new KPstateMainMenu;
        case KPState_Finish:
            return new KPstateFinish;
        case KPState_Settings:
            return new KPstateSettings;
        case KPState_ControlSettings:
            return new KPstateControlSettings;
        case KPState_GraphicSettings:
            return new KPstateGraphicSettings;
        case KPState_GraphicHint:
            return new KPstateGraphicHint;
        case KPState_AudioSettings:
            return new KPstateAudioSettings;
        case KPState_KeyboardHelp:
            return new KPstateKeyboardHelp;
        case KPState_Game:
            return new KPstateGame;
        case KPState_GameSolved:
            return new KPstateGameSolved;
        case KPState_SelectLanguage:
            return new KPstateSelectLanguage;
        case KPState_Tutorial1:
            return new KPstateTutorial1;
        case KPState_Tutorial2:
            return new KPstateTutorial2;
        case KPState_Tutorial3:
            return new KPstateTutorial3;
        case KPState_Tutorial4:
            return new KPstateTutorial4;
        case KPState_ScoreList:
            return new KPstateScoreList;
        case KPState_LightTest:
            return new KPstateLightTest;
        case KPState_Shutdown:
            return NULL; // pseudo state to shutdown the Application
    }
    DEBUGPRINT1("*** Error in State-factory: There is no state with ID %d "
                "defined\n", stateID);
    return NULL;
}

/////////////////////////////////////////////////////////////////////
// Constructor / Destructor
/////////////////////////////////////////////////////////////////////

KPstateFactory::KPstateFactory()
{
}

KPstateFactory::~KPstateFactory()
{
}
