/*
    kpstatesettings.h

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

    menu.SchildArray[SHLD_MENUBACKGROUND].Positioniere(4,2.5,12,8.5);
    menu.SchildArray[SHLD_LOGO].Positioniere(4,9,12,11);
    menu.SchildArray[SHLD_LOGO].VollSichtbar();

    menu.TextfeldArray[T_EINSTELLUNGEN]->Positioniere(8,8,1,A_MITTE);
    menu.TextfeldArray[T_EINSTELLUNGEN]->VollSichtbar();

    float y = 6.5;
    menu.TextfeldArray[T_STEUERUNG]->Positioniere(8,y,1,A_MITTE);
    y -= 1.0;
    menu.TextfeldArray[T_STEUERUNG]->SetzeSignal(S_CONTROL_SETTINGS);

    menu.TextfeldArray[T_GRAFIK   ]->Positioniere(8,y,1,A_MITTE);
    y -= 1.0;
    menu.TextfeldArray[T_GRAFIK   ]->SetzeSignal(S_GRAPHIC_SETTINGS);

    menu.TextfeldArray[T_AUDIO    ]->Positioniere(8,y,1,A_MITTE);
    y -= 1.0;
    menu.TextfeldArray[T_AUDIO    ]->SetzeSignal(S_AUDIO_SETTINGS);

    menu.TextfeldArray[T_SPRACHE  ]->Positioniere(8,y,1,A_MITTE);
    menu.TextfeldArray[T_SPRACHE  ]->SetzeSignal(S_SELECT_LANGUAGE);

    menu.TextfeldArray[T_ZURUECK  ]->Positioniere(8,1,1,A_MITTE);
    menu.TextfeldArray[T_ZURUECK  ]->SetzeSignal(S_BACK);

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
