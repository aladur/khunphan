/*
    kpstatesettings.cpp

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

#include "kpstatesettings.h"
#include "kpmenu.h"
#include "kpconfig.h"


void KPstateSettings::Initialize(KPstateContext *pContext,
                                 const KPstate *pOldState)
{
    KPstate::Initialize(pContext, pOldState);

    // Do some initialization stuff here:

    UpdateDisplay(pContext);
}

void KPstateSettings::UpdateDisplay(KPstateContext *pContext)
{
    KPstate::UpdateDisplay(pContext);

    KPmenu &menu = pContext->GetMenu();

    menu.plates[PLATE_MENUBACKGROUND].SetPosition(4,2.5,12,8.5);
    menu.plates[PLATE_LOGO].SetPosition(4,9,12,11);
    menu.plates[PLATE_LOGO].SetFullyVisible();

    menu.labels[T_SETTINGS].SetPosition(8,8,1,A_CENTERED);
    menu.labels[T_SETTINGS].SetFullyVisible();

    float y = 6.5;
    menu.labels[T_CONTROL].SetPosition(8,y,1,A_CENTERED);
    y -= 1.0;
    menu.labels[T_CONTROL].SetSignal(S_CONTROL_SETTINGS);

    menu.labels[T_GRAPHICS].SetPosition(8,y,1,A_CENTERED);
    y -= 1.0;
    menu.labels[T_GRAPHICS].SetSignal(S_GRAPHIC_SETTINGS);

    menu.labels[T_AUDIO].SetPosition(8,y,1,A_CENTERED);
    y -= 1.0;
    menu.labels[T_AUDIO].SetSignal(S_AUDIO_SETTINGS);

    menu.labels[T_LANGUAGE].SetPosition(8,y,1,A_CENTERED);
    menu.labels[T_LANGUAGE].SetSignal(S_SELECT_LANGUAGE);

    menu.labels[T_BACK].SetPosition(8,1,1,A_CENTERED);
    menu.labels[T_BACK].SetSignal(S_BACK);

    StartAnimation();
}

void  KPstateSettings::MouseClick (KPstateContext *pContext,
                                   tMouseButton button, tMouseEvent event,
                                   int x, int y)
{
    int Signal = KPstate::EvaluateMouseClick(pContext, button, event, x, y);

    switch (Signal)
    {
        case S_CONTROL_SETTINGS:
            pContext->ChangeState(KPState_ControlSettings);
            break;
        case S_GRAPHIC_SETTINGS:
            pContext->ChangeState(KPState_GraphicSettings);
            break;
        case S_AUDIO_SETTINGS:
            pContext->ChangeState(KPState_AudioSettings);
            break;
        case S_SELECT_LANGUAGE:
            pContext->ChangeState(KPState_SelectLanguage);
            break;
        case S_BACK:
            pContext->ChangeState(ESCKeyAction(pContext));
            break;
    }
}

void KPstateSettings::KeyPressed (KPstateContext *pContext, unsigned char key,
                                  int x, int y)
{
    CHECK_DEFAULT_KEY_PRESSED(pContext, key, x, y);
}

tKPMenuState KPstateSettings::ESCKeyAction (KPstateContext *pContext)
{
    /*  if (KPConfig::Instance().SavedGame != 0)
        return KPState_Game;
      else
        return KPState_MainMenu;
    */
    return pContext->GetMenu().RestoreLastStateId();
    //return oldStateId;
}
