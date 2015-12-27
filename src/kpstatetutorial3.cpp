/*
    kpstatetutorial3.cpp


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

#include "misc1.h"
#include "kpstatetutorial3.h"
#include "kpmenu.h"
#include "kpboardGL.h"
#include "camera.h"
#include "language.h"
#include "kpuibase.h"
#include "kpnodes.h"


KPstateTutorial3::KPstateTutorial3() : tutorialState(KP_TUT_STARTED)
{
}

void KPstateTutorial3::Initialize(KPstateContext *pContext,
                                  const KPstate *pOldState)
{
    KPstate::Initialize(pContext, pOldState);

    // Do some initialization stuff here:
    pContext->GetBoardView().EmphasizeToken( GetEmphasizedTokenId() );
    pContext->GetCamera().SetRoundtrip(false);
    InitializeBoardWithTokens(pContext);

    UpdateDisplay(pContext);
}

void KPstateTutorial3::UpdateDisplay(KPstateContext *pContext)
{
    KPstate::UpdateDisplay(pContext);

    KPmenu &menu = pContext->GetMenu();

    menu.plates[PLATE_MENUBACKGROUND].SetPosition(.5,4.5,15.5,11);

    menu.plates[PLATE_LOGO].SetPosition(0,11.0,4,12);
    menu.plates[PLATE_LOGO].SetSignal(S_BACK);
    menu.plates[PLATE_LOGO].SetFullyVisible();

    menu.labels[GetTextOffset()].SetPosition(8,10.5,1.0,A_CENTERED);
    menu.labels[GetTextOffset()].SetFullyVisible();

    float y = 9.5;
    menu.labels[GetTextOffset() + 2].SetPosition(1.25,y,0.5);
    y -= 0.5;
    if (menu.labels.find(GetTextOffset() + 3) !=
        menu.labels.end())
    {
        menu.labels[GetTextOffset() + 3].SetPosition(1.25,y,0.5);
    }
    y -= 0.5;
    if (menu.labels.find(GetTextOffset() + 4) !=
        menu.labels.end())
    {
        menu.labels[GetTextOffset() + 4].SetPosition(1.25,y,0.5);
    }
    y -= 0.5;
    if (menu.labels.find(GetTextOffset() + 5) !=
        menu.labels.end())
    {
        menu.labels[GetTextOffset() + 5].SetPosition(1.25,y,0.5);
    }
    y -= 0.5;
    if (menu.labels.find(GetTextOffset() + 6) !=
        menu.labels.end())
    {
        menu.labels[GetTextOffset() + 6].SetPosition(1.25,y,0.5);
    }
    y -= 0.5;
    if (menu.labels.find(GetTextOffset() + 7) !=
        menu.labels.end())
    {
        menu.labels[GetTextOffset() + 7].SetPosition(1.25,y,0.5);
    }
    y -= 0.5;

    switch (tutorialState)
    {
        case KP_TUT_RIGHT_TOKEN:
            // Klicked (and evtl. moved) the right token
            // Tutorial can be finished
            menu.labels[T_CONTINUE].SetPosition(8,1,1,A_CENTERED);
            menu.labels[T_CONTINUE].SetSignal(S_CONTINUE);
            menu.labels[GetTextOffset() + 1].SetPosition(1.25,y,0.5);
            y -= 0.5;
            break;
        case KP_TUT_NO_TOKEN:
            // Missed any token
            menu.labels[T_TUTORIAL34_EMPTY].SetPosition(1.25,y,0.5);
            y -= 0.5;
            menu.labels[T_TUTORIAL34_AGAIN].SetPosition(1.25,y,0.5);
            y -= 0.5;
            break;
        case KP_TUT_NOMOV_TOKEN:
            // Klicked on an unmovable token
            menu.labels[T_TUTORIAL34_NOMV].SetPosition(1.25,y,0.5);
            y -= 0.5;
            menu.labels[T_TUTORIAL34_AGAIN].SetPosition(1.25,y,0.5);
            y -= 0.5;
            break;
        case KP_TUT_WRONG_TOKEN:
            // Klicked on the wrong token
            menu.labels[T_TUTORIAL34_GOOD].SetPosition(1.25,y,0.5);
            y -= 0.5;
            menu.labels[T_TUTORIAL34_AGAIN].SetPosition(1.25,y,0.5);
            y -= 0.5;
            break;
        case KP_TUT_KLICKED_ONLY:
            // Klicked right token, but did not move with the mouse
            // This can only happen in Tutorial 4
            menu.labels[T_TUTORIAL4_MOVE].SetPosition(1.25,y,0.5);
            y -= 0.5;
            menu.labels[T_TUTORIAL34_AGAIN].SetPosition(1.25,y,0.5);
            break;
        case KP_TUT_STARTED:
        default:
            break;
    }

    StartAnimation();
}

void  KPstateTutorial3::MouseClick (KPstateContext *pContext,
                                    tMouseButton button, tMouseEvent event,
                                    int x, int y)
{
    int Signal = KPstate::EvaluateMouseClick(pContext, button, event, x, y);

    switch (Signal)
    {
        case S_CONTINUE:
            ContinueWithNextState(pContext);
            return;
        case S_BACK:
            pContext->ChangeState(ESCKeyAction(pContext));
            return;
    }

    if (tutorialState != KP_TUT_RIGHT_TOKEN)
    {
        MouseMoveToken(pContext, button, event, x, y);
    }
}

void  KPstateTutorial3::KeyPressed (KPstateContext *pContext, unsigned char key,
                                    int x, int y)
{
    CHECK_DEFAULT_KEY_PRESSED(pContext, key, x, y);

    switch (key)
    {
        case ' ':
        case 0x0d:
            ContinueWithNextState(pContext,
                                  tutorialState == KP_TUT_RIGHT_TOKEN);
            return;
    }
}

tKPMenuState KPstateTutorial3::ESCKeyAction (KPstateContext *pContext)
{
    const KPnode &node = pContext->GetNodes().GetRootNode();
    pContext->GetBoardView().SetBoard(node.GetBoard());
    pContext->GetBoardView().EmphasizeToken(TK_EMPTY);
    pContext->GetCamera().SetRoundtrip(true);

    return pContext->GetMenu().RestoreLastStateId();
}

void KPstateTutorial3::ContinueWithNextState(KPstateContext *pContext,
        bool valid /* = true */)
{
    if (!valid)
    {
        return;
    }

    const KPnode &node = pContext->GetNodes().GetRootNode();
    pContext->GetBoardView().SetBoard(node.GetBoard());
    pContext->GetBoardView().EmphasizeToken( TK_EMPTY );
    pContext->GetCamera().SetRoundtrip(true);
    pContext->ChangeState( KPState_Tutorial4 );
}

/////////////////////////////////////////////////////////////////////
// Check what has happened directly after
// Klicked on and evtl. moved a token (either successfully or not)
/////////////////////////////////////////////////////////////////////

void KPstateTutorial3::HookAfterTokenMoved(KPstateContext *pContext,
        tKPTokenID id,
        tKPDirection direction, bool successfullyMoved)
{
    tutorialState = KP_TUT_STARTED;

    if (!successfullyMoved && id == TK_EMPTY)
    {
        tutorialState = KP_TUT_NO_TOKEN;
    }
    else if (!successfullyMoved && id == GetEmphasizedTokenId() &&
             direction == MOVE_NO)
    {
        tutorialState = KP_TUT_KLICKED_ONLY;
    }
    else if (!successfullyMoved)
    {
        tutorialState = KP_TUT_NOMOV_TOKEN;
    }
    else if (id == GetEmphasizedTokenId())
    {
        tutorialState = KP_TUT_RIGHT_TOKEN;
        pContext->GetUserInterface().PlayAudio(KP_SND_MOVETOKEN);
    }
    else if (id != TK_EMPTY)
    {
        pContext->GetUserInterface().PlayAudio(KP_SND_MOVETOKEN);
        tutorialState = KP_TUT_WRONG_TOKEN;
    }

    UpdateDisplay(pContext);
}

/////////////////////////////////////////////////////////////////////
// Methods which can be overloaded dep. on the tutorial number
/////////////////////////////////////////////////////////////////////

int KPstateTutorial3::GetTextOffset()
{
    return T_TUTORIAL3;
}

tKPTokenID KPstateTutorial3::GetEmphasizedTokenId()
{
    return TK_GREEN1;
}

void KPstateTutorial3::InitializeBoardWithTokens(KPstateContext *pContext)
{
    KPboard board;

    board.InitializeToken(TK_GREEN1, 1, 4);
    board.InitializeToken(TK_GREEN2, 0, 4);
    board.InitializeToken(TK_GREEN3, 3, 4);
    board.InitializeToken(TK_GREEN4, 1, 3);
    board.InitializeToken(TK_WHITE1, 0, 0);
    board.InitializeToken(TK_WHITE2, 3, 0);
    board.InitializeToken(TK_WHITE3, 0, 2);
    board.InitializeToken(TK_WHITE4, 3, 2);
    board.InitializeToken(TK_WHITE5, 1, 2);
    board.InitializeToken(TK_RED1,   1, 0);

    pContext->GetBoardView().SetBoard(board);
}

void KPstateTutorial3::PlayAudioForInitialize(KPstateContext *pContext)
{
    pContext->GetUserInterface().PlayAudio(KP_SND_TUTORIALNEXT);
}


