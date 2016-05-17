/*
    kpstatetutorial3.cpp


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
#include "kpstatetutorial3.h"
#include "kpmenu.h"
#include "kpboardGL.h"
#include "camera.h"
#include "kpuibase.h"
#include "kpnodes.h"


KPstateTutorial3::KPstateTutorial3() : actionState(ActionState::Started)
{
}

void KPstateTutorial3::Initialize(KPstateContext *pContext,
                                  StateId previousStateId)
{
    KPstate::Initialize(pContext, previousStateId);

    pContext->GetBoardView().EmphasizeToken(GetEmphasizedTokenId());
    pContext->GetCamera().SetRoundtrip(false);
    InitializeBoardWithTokens(pContext);

    UpdateDisplay(pContext);
}

void KPstateTutorial3::UpdateDisplay(KPstateContext *pContext) const
{
    auto &menu = pContext->GetMenu();
    Lbl labelId;

    KPstate::UpdateDisplay(pContext);

    menu.plates[KPPlate::MenuBackground].SetPosition(.5, 4.5, 15.5, 11);

    menu.plates[KPPlate::Logo].SetPosition(0, 11.0, 4, 12);
    menu.plates[KPPlate::Logo].SetSignal(S_BACK);
    menu.plates[KPPlate::Logo].SetFullyVisible();

    labelId = GetTutorialLabelId();
    menu.labels[labelId].SetPosition(8, 10.5, 1.0, AlignItem::Centered);
    menu.labels[labelId].SetFullyVisible();

    auto y = 9.5f;
    std::size_t offset;

    for (offset = 2; offset <= 7; ++offset)
    {
        labelId = GetTutorialLabelId(offset);

        if (menu.labels.find(labelId) != menu.labels.end())
        {
            menu.labels[labelId].SetPosition(1.25, y, 0.5);
        }

        y -= 0.5;
    }

    switch (actionState)
    {
        case ActionState::RightTokenMoved:
            // Klicked (and evtl. moved) the right token
            // Tutorial can be finished
            menu.labels[Lbl::Continue].SetPosition(8, 1, 1,
                                                   AlignItem::Centered);
            menu.labels[Lbl::Continue].SetSignal(S_CONTINUE);
            menu.labels[GetTutorialLabelId(1)].SetPosition(1.25, y, 0.5);
            y -= 0.5;
            break;

        case ActionState::MissedAnyToken:
            // Missed any token
            menu.labels[Lbl::TutorialMissedToken].SetPosition(1.25, y, 0.5);
            y -= 0.5;
            menu.labels[Lbl::TutorialTryAgain].SetPosition(1.25, y, 0.5);
            y -= 0.5;
            break;

        case ActionState::TokenNotMoveable:
            // Klicked on an unmovable token
            menu.labels[Lbl::TutorialNotMoveable].SetPosition(1.25, y, 0.5);
            y -= 0.5;
            menu.labels[Lbl::TutorialTryAgain].SetPosition(1.25, y, 0.5);
            y -= 0.5;
            break;

        case ActionState::WrongTokenMoved:
            // Klicked on the wrong token
            menu.labels[Lbl::TutorialQuiteGood].SetPosition(1.25, y, 0.5);
            y -= 0.5;
            menu.labels[Lbl::TutorialTryAgain].SetPosition(1.25, y, 0.5);
            y -= 0.5;
            break;

        case ActionState::TokenKlickedOnly:
            // Klicked right token, but did not move with the mouse
            // This can only happen in Tutorial 4
            menu.labels[Lbl::TutorialMouseMove].SetPosition(1.25, y, 0.5);
            y -= 0.5;
            menu.labels[Lbl::TutorialTryAgain].SetPosition(1.25, y, 0.5);
            break;

        case ActionState::Started:
        default:
            break;
    }

    StartAnimation(pContext);
}

void  KPstateTutorial3::MouseClick(KPstateContext *pContext,
                                   MouseButton button, MouseButtonEvent event,
                                   int x, int y)
{
    auto Signal = KPstate::EvaluateMouseClick(pContext, button, event, x, y);

    switch (Signal)
    {
        case S_CONTINUE:
            ContinueWithNextState(pContext);
            return;

        case S_BACK:
            pContext->ChangeState(ESCKeyAction(pContext));
            return;
    }

    if (actionState != ActionState::RightTokenMoved)
    {
        MouseMoveToken(pContext, button, event, x, y);
    }
}

StateId KPstateTutorial3::ESCKeyAction(KPstateContext *pContext) const
{
    const auto &node = pContext->GetNodes().GetRootNode();
    pContext->GetBoardView().SetBoard(node.GetBoard());
    pContext->GetBoardView().EmphasizeToken(TokenId::EMPTY);
    pContext->GetCamera().SetRoundtrip(true);

    return pContext->GetMenu().RestoreLastStateId();
}

void KPstateTutorial3::ContinueWithNextState(KPstateContext *pContext,
        bool valid /* = true */) const
{
    if (!valid)
    {
        return;
    }

    const auto &node = pContext->GetNodes().GetRootNode();
    pContext->GetBoardView().SetBoard(node.GetBoard());
    pContext->GetBoardView().EmphasizeToken(TokenId::EMPTY);
    pContext->GetCamera().SetRoundtrip(true);
    pContext->ChangeState(StateId::Tutorial4);
}

/////////////////////////////////////////////////////////////////////
// Check what has happened directly after
// Klicked on and evtl. moved a token (either successfully or not)
/////////////////////////////////////////////////////////////////////

void KPstateTutorial3::HookAfterTokenMoved(KPstateContext *pContext,
        TokenId id,
        MoveToken direction, bool successfullyMoved)
{
    actionState = ActionState::Started;

    if (!successfullyMoved && id == TokenId::EMPTY)
    {
        actionState = ActionState::MissedAnyToken;
    }
    else if (!successfullyMoved && id == GetEmphasizedTokenId() &&
             direction == MoveToken::Not)
    {
        actionState = ActionState::TokenKlickedOnly;
    }
    else if (!successfullyMoved)
    {
        actionState = ActionState::TokenNotMoveable;
    }
    else if (id == GetEmphasizedTokenId())
    {
        actionState = ActionState::RightTokenMoved;
        pContext->GetUserInterface().PlayAudio(KPSound::MoveToken);
    }
    else if (id != TokenId::EMPTY)
    {
        pContext->GetUserInterface().PlayAudio(KPSound::MoveToken);
        actionState = ActionState::WrongTokenMoved;
    }

    UpdateDisplay(pContext);
}

/////////////////////////////////////////////////////////////////////
// Methods which can be overloaded dep. on the tutorial number
/////////////////////////////////////////////////////////////////////

Lbl KPstateTutorial3::GetTutorialLabelId(std::size_t offset) const
{
    return GetLabelId(Lbl::Tutorial3, offset);
}

TokenId KPstateTutorial3::GetEmphasizedTokenId() const
{
    return TokenId::GREEN1;
}

void KPstateTutorial3::InitializeBoardWithTokens(KPstateContext *pContext) const
{
    KPboard board;

    board.InitializeToken(TokenId::GREEN1, 1, 4);
    board.InitializeToken(TokenId::GREEN2, 0, 4);
    board.InitializeToken(TokenId::GREEN3, 3, 4);
    board.InitializeToken(TokenId::GREEN4, 1, 3);
    board.InitializeToken(TokenId::WHITE1, 0, 0);
    board.InitializeToken(TokenId::WHITE2, 3, 0);
    board.InitializeToken(TokenId::WHITE3, 0, 2);
    board.InitializeToken(TokenId::WHITE4, 3, 2);
    board.InitializeToken(TokenId::WHITE5, 1, 2);
    board.InitializeToken(TokenId::RED1,   1, 0);

    pContext->GetBoardView().SetBoard(board);
}

void KPstateTutorial3::PlayAudioForInitialize(KPstateContext *pContext) const
{
    pContext->GetUserInterface().PlayAudio(KPSound::TutorialNext);
}

