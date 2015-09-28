/*
    kpstatemainmenu.cpp

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

#include "kpstatemainmenu.h"
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

    menu.plates[PLATE_MENUBACKGROUND].SetPosition(2,2.5,14,8.5);

    menu.plates[PLATE_LOGO].SetPosition(5,9,11,11);
    menu.plates[PLATE_LOGO].SetFullyVisible();

    menu.labels[T_MAINMENU].SetPosition(8,8,1,A_CENTERED);
    menu.labels[T_MAINMENU].SetFullyVisible();

    menu.labels[T_TUTORIAL].SetPosition(5.5,6.5,1,A_CENTERED);
    menu.labels[T_TUTORIAL].SetSignal(S_TUTORIAL);

    menu.labels[T_H_KEYBOARDHELP].SetPosition(5.5,5,1,A_CENTERED);
    menu.labels[T_H_KEYBOARDHELP].SetSignal(S_KEYBOARD_HELP);

    menu.labels[T_SCORELIST].SetPosition(5.5,3.5,1,A_CENTERED);
    menu.labels[T_SCORELIST].SetSignal(S_SCORELIST);

    menu.labels[T_NEWGAME].SetPosition(10.5,6.5,1,A_CENTERED);
    menu.labels[T_NEWGAME].SetSignal(S_NEW_GAME);

    menu.labels[T_SETTINGS].SetPosition(10.5,5,1,A_CENTERED);
    menu.labels[T_SETTINGS].SetSignal(S_SETTINGS);

    if (KPConfig::Instance().SavedGame != 0)
    {
        menu.labels[T_CONTINUEGAME].SetPosition(10.5,3.5,1,A_CENTERED);
        menu.labels[T_CONTINUEGAME].SetSignal(S_CONTINUE);
    }

    menu.labels[T_QUIT].SetPosition(8,1,1,A_CENTERED);
    menu.labels[T_QUIT].SetSignal(S_FINISH);

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