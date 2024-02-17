/*
    kpstatemainmenu.cpp

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
    menu.labels[Lbl::Tutorial].SetSignal(Signal::Tutorial);

    menu.labels[Lbl::KeyboardHelp].SetPosition(5.5, 5, 1, AlignItem::Centered);
    menu.labels[Lbl::KeyboardHelp].SetSignal(Signal::KeyboardHelp);

    menu.labels[Lbl::ScoreList].SetPosition(5.5, 3.5, 1, AlignItem::Centered);
    menu.labels[Lbl::ScoreList].SetSignal(Signal::ScoreList);

    menu.labels[Lbl::NewGame].SetPosition(10.5, 6.5, 1, AlignItem::Centered);
    menu.labels[Lbl::NewGame].SetSignal(Signal::NewGame);

    menu.labels[Lbl::Settings].SetPosition(10.5, 5, 1, AlignItem::Centered);
    menu.labels[Lbl::Settings].SetSignal(Signal::Settings);

    if (pContext->GetConfig().SavedGame != 0)
    {
        menu.labels[Lbl::ContinueGame].SetPosition(10.5, 3.5, 1,
                AlignItem::Centered);
        menu.labels[Lbl::ContinueGame].SetSignal(Signal::Continue);
    }

    menu.labels[Lbl::Quit].SetPosition(8, 1, 1, AlignItem::Centered);
    menu.labels[Lbl::Quit].SetSignal(Signal::Quit);

    StartAnimation(pContext);
}

void  KPstateMainMenu::MouseClick(KPstateContext *pContext,
                                  MouseButton button, MouseButtonEvent event,
                                  int x, int y)
{
    auto signal = KPstate::EvaluateMouseClick(pContext, button, event, x, y);

    switch (signal)
    {
        case Signal::NewGame:
            pContext->GetConfig().SavedGame = 0;

        // Intentionally fall through next case
        case Signal::Continue:
            pContext->ChangeState(StateId::Game);
            return;

        case Signal::Tutorial:
            pContext->ChangeState(StateId::Tutorial1);
            return;

        case Signal::KeyboardHelp:
            pContext->ChangeState(StateId::KeyboardHelp);
            return;

        case Signal::Settings:
            pContext->ChangeState(StateId::Settings);
            return;

        case Signal::ScoreList:
            pContext->ChangeState(StateId::ScoreList);
            return;

        case Signal::Quit:
            pContext->ChangeState(StateId::Finish);
            return;

        default:
            break;
    }
}

StateId KPstateMainMenu::ESCKeyAction(KPstateContext *) const
{
    return StateId::Finish;
}
