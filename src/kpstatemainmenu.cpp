/*
    kpstatemainmenu.cpp

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
#include "kpstatemainmenu.h"
#include "kpmenu.h"
#include "kpboardGL.h"
#include "kpstatistics.h"
#include "kpconfig.h"


void KPstateMainMenu::Initialize(KPstateContext *pContext,
                                 StateId previousStateId)
{
    KPstate::Initialize(pContext, previousStateId);

    if (previousStateId == StateId::Game)
    {
        pContext->GetMenu().SaveLastStateId(previousStateId);
    }
    else
    {
        pContext->GetMenu().SaveLastStateId(this->GetId());
    }

    UpdateDisplay(pContext);
}

void KPstateMainMenu::UpdateDisplay(KPstateContext *pContext) const
{
    auto &menu = pContext->GetMenu();

    KPstate::UpdateDisplay(pContext);

    // every thing which should be permanently be visible within this
    // state should be listed here:

    menu.plates[KPPlate::MenuBackground].SetPosition(2, 2.5, 14, 8.5);

    menu.plates[KPPlate::Logo].SetPosition(5, 9, 11, 11);
    menu.plates[KPPlate::Logo].SetFullyVisible();

    menu.labels[Lbl::MainMenu].SetPosition(8, 8, 1, AlignItem::Centered);
    menu.labels[Lbl::MainMenu].SetFullyVisible();

    menu.labels[Lbl::Tutorial].SetPosition(5.5, 6.5, 1, AlignItem::Centered);
    menu.labels[Lbl::Tutorial].SetSignal(S_TUTORIAL);

    menu.labels[Lbl::KeyboardHelp].SetPosition(5.5, 5, 1, AlignItem::Centered);
    menu.labels[Lbl::KeyboardHelp].SetSignal(S_KEYBOARD_HELP);

    menu.labels[Lbl::ScoreList].SetPosition(5.5, 3.5, 1, AlignItem::Centered);
    menu.labels[Lbl::ScoreList].SetSignal(S_SCORELIST);

    menu.labels[Lbl::NewGame].SetPosition(10.5, 6.5, 1, AlignItem::Centered);
    menu.labels[Lbl::NewGame].SetSignal(S_NEW_GAME);

    menu.labels[Lbl::Settings].SetPosition(10.5, 5, 1, AlignItem::Centered);
    menu.labels[Lbl::Settings].SetSignal(S_SETTINGS);

    if (pContext->GetConfig().SavedGame != 0)
    {
        menu.labels[Lbl::ContinueGame].SetPosition(10.5, 3.5, 1,
                AlignItem::Centered);
        menu.labels[Lbl::ContinueGame].SetSignal(S_CONTINUE);
    }

    menu.labels[Lbl::Quit].SetPosition(8, 1, 1, AlignItem::Centered);
    menu.labels[Lbl::Quit].SetSignal(S_FINISH);

    StartAnimation(pContext);
}

void  KPstateMainMenu::MouseClick(KPstateContext *pContext,
                                  MouseButton button, MouseButtonEvent event,
                                  int x, int y)
{
    auto Signal = KPstate::EvaluateMouseClick(pContext, button, event, x, y);

    switch (Signal)
    {
        case S_NEW_GAME:
            pContext->GetConfig().SavedGame = 0;

        case S_CONTINUE:
            pContext->ChangeState(StateId::Game);
            break;

        case S_TUTORIAL:
            pContext->ChangeState(StateId::Tutorial1);
            break;

        case S_KEYBOARD_HELP:
            pContext->ChangeState(StateId::KeyboardHelp);
            break;

        case S_SETTINGS:
            pContext->ChangeState(StateId::Settings);
            break;

        case S_SCORELIST:
            pContext->ChangeState(StateId::ScoreList);
            break;

        case S_FINISH:
            pContext->ChangeState(StateId::Finish);
            break;
    }
}

StateId KPstateMainMenu::ESCKeyAction(KPstateContext *) const
{
    return StateId::Finish;
}
