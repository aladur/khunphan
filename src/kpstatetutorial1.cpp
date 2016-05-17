/*
    kpstatetutorial1.cpp


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
#include "kpstatetutorial1.h"
#include "kpmenu.h"
#include "kpboardGL.h"
#include "camera.h"
#include "kpuibase.h"
#include "kpboard.h"


void KPstateTutorial1::Initialize(KPstateContext *pContext,
                                  StateId previousStateId)
{
    auto &menu = pContext->GetMenu();

    KPstate::Initialize(pContext, previousStateId);

    menu.SaveLastStateId(previousStateId);

    pContext->GetBoardView().EmphasizeToken(TokenId::RED1);
    InitializeBoardWithTokens(pContext);

    // Set Tutorial-Text to width  31.2
    for (auto labelId = Lbl::Tutorial1; labelId != Lbl::TutorialEnd;)
    {
        if (menu.labels.find(labelId) != menu.labels.end())
        {
            menu.labels[labelId].SetMaxWidth(31);
        }

        labelId = GetLabelId(labelId, 1);
    }

    pContext->GetCamera().SetRoundtrip(false);
    pContext->GetCamera().SetPosition(2);

    UpdateDisplay(pContext);
}

void KPstateTutorial1::UpdateDisplay(KPstateContext *pContext) const
{
    auto &menu = pContext->GetMenu();

    KPstate::UpdateDisplay(pContext);

    menu.plates[KPPlate::MenuBackground].SetPosition(.5, 4.0, 15.5, 11);

    menu.plates[KPPlate::Logo].SetPosition(0, 11.0, 4, 12);
    menu.plates[KPPlate::Logo].SetSignal(S_BACK);
    menu.plates[KPPlate::Logo].SetFullyVisible();

    menu.labels[Lbl::Tutorial1].SetPosition(8, 10.5, 1.0, AlignItem::Centered);
    menu.labels[Lbl::Tutorial1].SetFullyVisible();

    auto y = 9.5f;
    menu.labels[Lbl::Tutorial11].SetPosition(1.25, y, 0.7f);
    y -= 1.0;
    menu.labels[Lbl::Tutorial12].SetPosition(1.25, y, 0.5);
    y -= 0.5;

    if (menu.labels.find(Lbl::Tutorial13) != menu.labels.end())
    {
        menu.labels[Lbl::Tutorial13].SetPosition(1.25, y, 0.5);
    }

    y -= 0.5;

    if (menu.labels.find(Lbl::Tutorial14) != menu.labels.end())
    {
        menu.labels[Lbl::Tutorial14].SetPosition(1.25, y, 0.5);
    }

    y -= 0.5;

    if (menu.labels.find(Lbl::Tutorial15) != menu.labels.end())
    {
        menu.labels[Lbl::Tutorial15].SetPosition(1.25, y, 0.5);
    }

    y -= 0.5;

    if (menu.labels.find(Lbl::Tutorial16) != menu.labels.end())
    {
        menu.labels[Lbl::Tutorial16].SetPosition(1.25, y, 0.5);
    }

    y -= 0.5;

    if (menu.labels.find(Lbl::Tutorial17) != menu.labels.end())
    {
        menu.labels[Lbl::Tutorial17].SetPosition(1.25, y, 0.5);
    }

    y -= 0.5;

    if (menu.labels.find(Lbl::Tutorial18) != menu.labels.end())
    {
        menu.labels[Lbl::Tutorial18].SetPosition(1.25, y, 0.5);
    }

    y -= 0.5;

    if (menu.labels.find(Lbl::Tutorial19) != menu.labels.end())
    {
        menu.labels[Lbl::Tutorial19].SetPosition(1.25, y, 0.5);
    }

    menu.labels[Lbl::Continue].SetPosition(8, 1, 1, AlignItem::Centered);
    menu.labels[Lbl::Continue].SetSignal(S_CONTINUE);

    StartAnimation(pContext);
}

void  KPstateTutorial1::MouseClick(KPstateContext *pContext,
                                   MouseButton button, MouseButtonEvent event,
                                   int x, int y)
{
    auto Signal = KPstate::EvaluateMouseClick(pContext, button, event, x, y);

    switch (Signal)
    {
        case S_CONTINUE:
            pContext->ChangeState(StateId::Tutorial2);
            break;

        case S_BACK:
            pContext->ChangeState(ESCKeyAction(pContext));
            break;
    }
}

StateId KPstateTutorial1::ESCKeyAction(KPstateContext *pContext) const
{
    pContext->GetCamera().SetRoundtrip(true);
    pContext->GetBoardView().EmphasizeToken(TokenId::EMPTY);
    return pContext->GetMenu().RestoreLastStateId();
}

void KPstateTutorial1::InitializeBoardWithTokens(KPstateContext *pContext) const
{
    KPboard board;

    board.InitializeToken(TokenId::GREEN1, 1, 3);
    board.InitializeToken(TokenId::GREEN2, 2, 3);
    board.InitializeToken(TokenId::GREEN3, 1, 4);
    board.InitializeToken(TokenId::GREEN4, 2, 4);
    board.InitializeToken(TokenId::WHITE1, 0, 0);
    board.InitializeToken(TokenId::WHITE2, 3, 0);
    board.InitializeToken(TokenId::WHITE3, 0, 3);
    board.InitializeToken(TokenId::WHITE4, 3, 3);
    board.InitializeToken(TokenId::WHITE5, 1, 2);
    board.InitializeToken(TokenId::RED1,   1, 0);

    pContext->GetBoardView().SetBoard(board);
}

void KPstateTutorial1::PlayAudioForInitialize(KPstateContext *pContext) const
{
    pContext->GetUserInterface().PlayAudio(KPSound::TutorialNext);
}
