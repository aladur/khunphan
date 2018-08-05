/*
    kpstatetutorial2.cpp


    Automatic solution finder for KhunPhan game
    Copyright (C) 2001-2018  W. Schwotzer

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
#include "kpstatetutorial2.h"
#include "kpmenu.h"
#include "kpboardGL.h"
#include "camera.h"
#include "kpuibase.h"
#include "kpboard.h"


void KPstateTutorial2::Initialize(KPstateContext *pContext,
                                  StateId previousStateId)
{
    auto &menu = pContext->GetMenu();

    KPstate::Initialize(pContext, previousStateId);

    pContext->GetBoardView().EmphasizeToken(TokenId::RED1);
    pContext->GetCamera().SetRoundtrip(false);
    InitializeBoardWithTokens(pContext);

    menu.labels[Lbl::Tutorial2Hint].SetMaxWidth(13.5f);
    menu.labels[Lbl::Tutorial2Hint].SetLineSpacing(1.0f);

    UpdateDisplay(pContext);
}

void KPstateTutorial2::UpdateDisplay(KPstateContext *pContext) const
{
    auto &menu = pContext->GetMenu();

    KPstate::UpdateDisplay(pContext);

    menu.plates[KPPlate::MenuBackground].SetPosition(.5, 4.5, 15.5, 11);

    menu.plates[KPPlate::Logo].SetPosition(0, 11.0, 4, 12);
    menu.plates[KPPlate::Logo].SetSignal(Signal::Back);
    menu.plates[KPPlate::Logo].SetFullyVisible();

    menu.labels[Lbl::Tutorial2].SetPosition(8, 10.5, 1.0, AlignItem::Centered);
    menu.labels[Lbl::Tutorial2].SetFullyVisible();

    menu.labels[Lbl::Tutorial2Hint].SetPosition(1.25, 9.5f, 0.5);

    menu.labels[Lbl::Continue].SetPosition(8, 1, 1, AlignItem::Centered);
    menu.labels[Lbl::Continue].SetSignal(Signal::Continue);

    StartAnimation(pContext);
}

StateId KPstateTutorial2::ESCKeyAction(KPstateContext *pContext) const
{
    pContext->GetCamera().SetRoundtrip(true);
    pContext->GetBoardView().EmphasizeToken(TokenId::EMPTY);
    return pContext->GetMenu().RestoreLastStateId();
}

void  KPstateTutorial2::MouseClick(KPstateContext *pContext,
                                   MouseButton button, MouseButtonEvent event,
                                   int x, int y)
{
    auto signal = KPstate::EvaluateMouseClick(pContext, button, event, x, y);

    switch (signal)
    {
        case Signal::Continue:
            pContext->ChangeState(StateId::Tutorial3);
            return;

        case Signal::Back:
            pContext->ChangeState(ESCKeyAction(pContext));
            return;

        default:
            break;
    }
}

void KPstateTutorial2::InitializeBoardWithTokens(KPstateContext *pContext) const
{
    KPboard board;

    board.InitializeToken(TokenId::GREEN1, 0, 0);
    board.InitializeToken(TokenId::GREEN2, 3, 3);
    board.InitializeToken(TokenId::GREEN3, 3, 4);
    board.InitializeToken(TokenId::GREEN4, 1, 2);
    board.InitializeToken(TokenId::WHITE1, 0, 1);
    board.InitializeToken(TokenId::WHITE2, 1, 0);
    board.InitializeToken(TokenId::WHITE3, 2, 0);
    board.InitializeToken(TokenId::WHITE4, 3, 0);
    board.InitializeToken(TokenId::WHITE5, 2, 2);
    board.InitializeToken(TokenId::RED1,   1, 3);

    pContext->GetBoardView().SetBoard(board);
}

void KPstateTutorial2::PlayAudioForInitialize(KPstateContext *pContext) const
{
    pContext->GetUserInterface().PlayAudio(KPSound::TutorialNext);
}
