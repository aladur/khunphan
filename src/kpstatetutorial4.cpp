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

#include "kpstatetutorial4.h"
#include "kpboardGL.h"
#include "kpmenu.h"
#include "camera.h"
#include "kpuibase.h"
#include "kpboard.h"


void KPstateTutorial4::ContinueWithNextState(KPstateContext *pContext,
        bool valid /* = true */)
{
    if (!valid)
    {
        return;
    }

    pContext->GetCamera().SetRoundtrip(true);
    pContext->GetBoardView().EmphasizeToken( TK_EMPTY );
    pContext->ChangeState( pContext->GetMenu().RestoreLastStateId() );
}

int KPstateTutorial4::GetTextOffset()
{
    return T_TUTORIAL4;
}

tKPTokenID KPstateTutorial4::GetEmphasizedTokenId()
{
    return TK_GREEN4;
}

void KPstateTutorial4::InitializeBoardWithTokens(KPstateContext *pContext)
{
    KPboard board;

    board.InitializeToken(TK_GREEN1, 1, 4);
    board.InitializeToken(TK_GREEN2, 0, 4);
    board.InitializeToken(TK_GREEN3, 1, 3);
    board.InitializeToken(TK_GREEN4, 2, 4);
    board.InitializeToken(TK_WHITE1, 0, 0);
    board.InitializeToken(TK_WHITE2, 3, 0);
    board.InitializeToken(TK_WHITE3, 0, 2);
    board.InitializeToken(TK_WHITE4, 3, 2);
    board.InitializeToken(TK_WHITE5, 1, 2);
    board.InitializeToken(TK_RED1,   1, 0);

    pContext->GetBoardView().SetBoard(board);
}

void KPstateTutorial4::PlayAudioForInitialize(KPstateContext *pContext)
{
    pContext->GetUserInterface().PlayAudio(KP_SND_TUTORIALNEXT);
}
