/*
    kpstatetutorial2.cpp


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
#include "kpstatetutorial2.h"
#include "kpmenu.h"
#include "kpboardGL.h"
#include "camera.h"
#include "kpuibase.h"
#include "kpboard.h"


void KPstateTutorial2::Initialize(KPstateContext *pContext,
                                  tKPMenuState previousStateId)
{
    KPstate::Initialize(pContext, previousStateId);

    pContext->GetBoardView().EmphasizeToken(TK_RED1);
    pContext->GetCamera().SetRoundtrip(false);
    InitializeBoardWithTokens(pContext);

    UpdateDisplay(pContext);
}

void KPstateTutorial2::UpdateDisplay(KPstateContext *pContext) const
{
    auto &menu = pContext->GetMenu();

    KPstate::UpdateDisplay(pContext);

    menu.plates[KPPlate::MenuBackground].SetPosition(.5, 4.5, 15.5, 11);

    menu.plates[KPPlate::Logo].SetPosition(0, 11.0, 4, 12);
    menu.plates[KPPlate::Logo].SetSignal(S_BACK);
    menu.plates[KPPlate::Logo].SetFullyVisible();

    menu.labels[T_TUTORIAL2].SetPosition(8, 10.5, 1.0, AlignItem::Centered);
    menu.labels[T_TUTORIAL2].SetFullyVisible();

    auto y = 9.5f;
    menu.labels[T_TUTORIAL2_01].SetPosition(1.25, y, 0.5);
    y -= 0.5;

    if (menu.labels.find(T_TUTORIAL2_02) != menu.labels.end())
    {
        menu.labels[T_TUTORIAL2_02].SetPosition(1.25, y, 0.5);
    }

    y -= 0.5;

    if (menu.labels.find(T_TUTORIAL2_03) != menu.labels.end())
    {
        menu.labels[T_TUTORIAL2_03].SetPosition(1.25, y, 0.5);
    }

    y -= 0.5;

    if (menu.labels.find(T_TUTORIAL2_04) != menu.labels.end())
    {
        menu.labels[T_TUTORIAL2_04].SetPosition(1.25, y, 0.5);
    }

    y -= 0.5;

    if (menu.labels.find(T_TUTORIAL2_05) != menu.labels.end())
    {
        menu.labels[T_TUTORIAL2_05].SetPosition(1.25, y, 0.5);
    }

    menu.labels[T_CONTINUE].SetPosition(8, 1, 1, AlignItem::Centered);
    menu.labels[T_CONTINUE].SetSignal(S_CONTINUE);

    StartAnimation(pContext);
}

tKPMenuState KPstateTutorial2::ESCKeyAction(KPstateContext *pContext) const
{
    pContext->GetCamera().SetRoundtrip(true);
    pContext->GetBoardView().EmphasizeToken(TK_EMPTY);
    return pContext->GetMenu().RestoreLastStateId();
}

void  KPstateTutorial2::MouseClick(KPstateContext *pContext,
                                   MouseButton button, MouseButtonEvent event,
                                   int x, int y)
{
    auto Signal = KPstate::EvaluateMouseClick(pContext, button, event, x, y);

    switch (Signal)
    {
        case S_CONTINUE:
            pContext->ChangeState(KPState_Tutorial3);
            break;

        case S_BACK:
            pContext->ChangeState(ESCKeyAction(pContext));
            break;
    }
}

void KPstateTutorial2::InitializeBoardWithTokens(KPstateContext *pContext) const
{
    KPboard board;

    board.InitializeToken(TK_GREEN1, 0, 0);
    board.InitializeToken(TK_GREEN2, 3, 3);
    board.InitializeToken(TK_GREEN3, 3, 4);
    board.InitializeToken(TK_GREEN4, 1, 2);
    board.InitializeToken(TK_WHITE1, 0, 1);
    board.InitializeToken(TK_WHITE2, 1, 0);
    board.InitializeToken(TK_WHITE3, 2, 0);
    board.InitializeToken(TK_WHITE4, 3, 0);
    board.InitializeToken(TK_WHITE5, 2, 2);
    board.InitializeToken(TK_RED1,   1, 3);

    pContext->GetBoardView().SetBoard(board);
}

void KPstateTutorial2::PlayAudioForInitialize(KPstateContext *pContext) const
{
    pContext->GetUserInterface().PlayAudio(KPSound::TutorialNext);
}
