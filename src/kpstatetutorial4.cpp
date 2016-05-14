/*
    kpstatetutorial4.cpp


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
#include "kpstatetutorial4.h"
#include "kpboardGL.h"
#include "kpmenu.h"
#include "camera.h"
#include "kpuibase.h"
#include "kpboard.h"


void KPstateTutorial4::ContinueWithNextState(KPstateContext *pContext,
        bool valid /* = true */) const
{
    if (!valid)
    {
        return;
    }

    pContext->GetCamera().SetRoundtrip(true);
    pContext->GetBoardView().EmphasizeToken(TokenId::EMPTY);
    pContext->ChangeState(pContext->GetMenu().RestoreLastStateId());
}

int KPstateTutorial4::GetTextOffset() const
{
    return T_TUTORIAL4;
}

TokenId KPstateTutorial4::GetEmphasizedTokenId() const
{
    return TokenId::GREEN4;
}

void KPstateTutorial4::InitializeBoardWithTokens(KPstateContext *pContext) const
{
    KPboard board;

    board.InitializeToken(TokenId::GREEN1, 1, 4);
    board.InitializeToken(TokenId::GREEN2, 0, 4);
    board.InitializeToken(TokenId::GREEN3, 1, 3);
    board.InitializeToken(TokenId::GREEN4, 2, 4);
    board.InitializeToken(TokenId::WHITE1, 0, 0);
    board.InitializeToken(TokenId::WHITE2, 3, 0);
    board.InitializeToken(TokenId::WHITE3, 0, 2);
    board.InitializeToken(TokenId::WHITE4, 3, 2);
    board.InitializeToken(TokenId::WHITE5, 1, 2);
    board.InitializeToken(TokenId::RED1,   1, 0);

    pContext->GetBoardView().SetBoard(board);
}

void KPstateTutorial4::PlayAudioForInitialize(KPstateContext *pContext) const
{
    pContext->GetUserInterface().PlayAudio(KPSound::TutorialNext);
}
