/*
    kpstatetutorial4.cpp


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

#include "kpstatetutorial4.h"
#include "kpboardGL.h"
#include "kpmenu.h"
#include "camera.h"
#include "kpuibase.h"


void KPstateTutorial4::ContinueWithNextState(KPstateContext *pContext,
        bool valid /* = true */)
{
    if (!valid)
    {
        return;
    }

    pContext->GetCamera().SetzeRundflug(true);
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
    KPnode n;

    n.InitializeToken(TK_GREEN1, "green", 1, 4, 1, 1);
    n.InitializeToken(TK_GREEN2, "green", 0, 4, 1, 1);
    n.InitializeToken(TK_GREEN3, "green", 1, 3, 1, 1);
    n.InitializeToken(TK_GREEN4, "green", 2, 4, 1, 1);
    n.InitializeToken(TK_WHITE1, "white", 0, 0, 1, 2);
    n.InitializeToken(TK_WHITE2, "white", 3, 0, 1, 2);
    n.InitializeToken(TK_WHITE3, "white", 0, 2, 1, 2);
    n.InitializeToken(TK_WHITE4, "white", 3, 2, 1, 2);
    n.InitializeToken(TK_WHITE5, "white", 1, 2, 2, 1);
    n.InitializeToken(TK_RED1,   "red",   1, 0, 2, 2);

    pContext->GetBoardView().SetBoard(&n);
}

void KPstateTutorial4::PlayAudioForInitialize(KPstateContext *pContext)
{
    pContext->GetUserInterface().PlayAudio(KP_SND_TUTORIALNEXT);
}
