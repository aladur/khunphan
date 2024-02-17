/*
    kpstatefactory.cpp

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
#include <stdexcept>
#include <memory>
#include "kpstatefactory.h"
#include "kpstate.h"
#include "kpstatestartup.h"
#include "kpstatemainmenu.h"
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


/////////////////////////////////////////////////////////////////////
// Public Interface
/////////////////////////////////////////////////////////////////////

std::unique_ptr<KPstate> KPstateFactory::CreateState(StateId stateID)
{
    std::unique_ptr<KPstate> statePtr;

    switch (stateID)
    {
        case StateId::StartUp:
            statePtr.reset(new KPstateStartUp);
            break;

        case StateId::MainMenu:
            statePtr.reset(new KPstateMainMenu);
            break;

        case StateId::Finish:
            statePtr.reset(new KPstateFinish);
            break;

        case StateId::Settings:
            statePtr.reset(new KPstateSettings);
            break;

        case StateId::ControlSettings:
            statePtr.reset(new KPstateControlSettings);
            break;

        case StateId::GraphicSettings:
            statePtr.reset(new KPstateGraphicSettings);
            break;

        case StateId::GraphicHint:
            statePtr.reset(new KPstateGraphicHint);
            break;

        case StateId::AudioSettings:
            statePtr.reset(new KPstateAudioSettings);
            break;

        case StateId::KeyboardHelp:
            statePtr.reset(new KPstateKeyboardHelp);
            break;

        case StateId::Game:
            statePtr.reset(new KPstateGame);
            break;

        case StateId::GameSolved:
            statePtr.reset(new KPstateGameSolved);
            break;

        case StateId::SelectLanguage:
            statePtr.reset(new KPstateSelectLanguage);
            break;

        case StateId::Tutorial1:
            statePtr.reset(new KPstateTutorial1);
            break;

        case StateId::Tutorial2:
            statePtr.reset(new KPstateTutorial2);
            break;

        case StateId::Tutorial3:
            statePtr.reset(new KPstateTutorial3);
            break;

        case StateId::Tutorial4:
            statePtr.reset(new KPstateTutorial4);
            break;

        case StateId::ScoreList:
            statePtr.reset(new KPstateScoreList);
            break;

        case StateId::LightTest:
            statePtr.reset(new KPstateLightTest);
            break;

        default:
            std::stringstream message;

            message << "Error in KPstateFactory::CreateState: Unknown state "
                    << static_cast<std::size_t>(stateID);
            throw std::runtime_error(message.str());
    }

    return statePtr;
}

