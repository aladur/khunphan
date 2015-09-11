/*
    kpstatemeinmenu.h

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

#include "kpstatemeinmenu.h"
#include "kpmenu.h"
#include "kpboardGL.h"
#include "kpstatistics.h"
#include "kpconfig.h"


void KPstateMainMenu::Initialize(KPstateContext *pContext,
                                 const KPstate *pOldState)
{
    KPstate::Initialize(pContext, pOldState);

    // Do some initialization stuff here:
    if (pOldState->GetId() == KPState_Game)
    {
        pContext->GetMenu().SaveLastStateId(pOldState->GetId());
    }
    else
    {
        pContext->GetMenu().SaveLastStateId(this->GetId());
    }

    UpdateDisplay(pContext);
}

void KPstateMainMenu::UpdateDisplay(KPstateContext *pContext)
{
    KPstate::UpdateDisplay(pContext);

    KPmenu &menu = pContext->GetMenu();

    // every thing which should be permanently be visible within this
    // state should be listed here:

    menu.SchildArray[SHLD_MENUBACKGROUND]->Positioniere(2,2.5,14,8.5);

    menu.SchildArray[SHLD_LOGO]->Positioniere(5,9,11,11);
    menu.SchildArray[SHLD_LOGO]->VollSichtbar();

    menu.TextfeldArray[T_HAUPTMENU     ]->Positioniere(8,8,1,A_MITTE);
    menu.TextfeldArray[T_HAUPTMENU     ]->VollSichtbar();

    menu.TextfeldArray[T_TUTORIAL      ]->Positioniere(5.5,6.5,1,A_MITTE);
    menu.TextfeldArray[T_TUTORIAL      ]->SetzeSignal(S_TUTORIAL);

    menu.TextfeldArray[T_H_KEYBOARDHELP]->Positioniere(5.5,5,1,A_MITTE);
    menu.TextfeldArray[T_H_KEYBOARDHELP]->SetzeSignal(S_KEYBOARD_HELP);

    menu.TextfeldArray[T_SCORELIST     ]->Positioniere(5.5,3.5,1,A_MITTE);
    menu.TextfeldArray[T_SCORELIST     ]->SetzeSignal(S_SCORELIST);

    menu.TextfeldArray[T_NEUESSPIEL    ]->Positioniere(10.5,6.5,1,A_MITTE);
    menu.TextfeldArray[T_NEUESSPIEL    ]->SetzeSignal(S_NEW_GAME);

    menu.TextfeldArray[T_EINSTELLUNGEN ]->Positioniere(10.5,5,1,A_MITTE);
    menu.TextfeldArray[T_EINSTELLUNGEN ]->SetzeSignal(S_SETTINGS);

    if (KPConfig::Instance().SavedGame != 0)
    {
        menu.TextfeldArray[T_WEITERSPIELEN]->Positioniere(10.5,3.5,1,A_MITTE);
        menu.TextfeldArray[T_WEITERSPIELEN]->SetzeSignal(S_CONTINUE);
    }

    menu.TextfeldArray[T_BEENDEN       ]->Positioniere(8,1,1,A_MITTE);
    menu.TextfeldArray[T_BEENDEN       ]->SetzeSignal(S_FINISH);

    StartAnimation();
}

void  KPstateMainMenu::KeyPressed (KPstateContext *pContext, unsigned char key,
                                   int x, int y)
{
    CHECK_DEFAULT_KEY_PRESSED(pContext, key, x, y);
}

void  KPstateMainMenu::MouseClick (KPstateContext *pContext,
                                   tMouseButton button, tMouseEvent event,
                                   int x, int y)
{
    int Signal = KPstate::EvaluateMouseClick(pContext, button, event, x, y);

    switch (Signal)
    {
        case S_NEW_GAME:
            KPConfig::Instance().SavedGame = 0;
        case S_CONTINUE:
            pContext->ChangeState(KPState_Game);
            break;
        case S_TUTORIAL:
            pContext->ChangeState(KPState_Tutorial1);
            break;
        case S_KEYBOARD_HELP:
            pContext->ChangeState(KPState_KeyboardHelp);
            break;
        case S_SETTINGS:
            pContext->ChangeState(KPState_Settings);
            break;
        case S_SCORELIST:
            pContext->ChangeState(KPState_ScoreList);
            break;
        case S_FINISH:
            pContext->ChangeState(KPState_Finish);
            break;
    }
}

tKPMenuState KPstateMainMenu::ESCKeyAction (KPstateContext *)
{
    return KPState_Finish;
}
