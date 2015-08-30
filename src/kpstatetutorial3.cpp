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
#include "Kamera.h"
#include "sprache.h"
#include "kpuibase.h"


KPstateTutorial3::KPstateTutorial3() : tutorialState(KP_TUT_STARTED)
{
}

void KPstateTutorial3::Initialize(KPstateContext *pContext,  const KPstate *pOldState)
{
  KPstate::Initialize(pContext, pOldState);

  // Do some initialization stuff here:
  pContext->GetBoardView().EmphasizeToken( GetEmphasizedTokenId() );
  pContext->GetCamera().SetzeRundflug(false);
  InitializeBoardWithTokens(pContext);

  UpdateDisplay(pContext);
}

void KPstateTutorial3::UpdateDisplay(KPstateContext *pContext)
{
  KPstate::UpdateDisplay(pContext);

  KPmenu &menu = pContext->GetMenu();

  menu.SchildArray[SHLD_MENUBACKGROUND]->Positioniere(.5,4.5,15.5,11);

  menu.SchildArray[SHLD_LOGO]->Positioniere(0,11.0,4,12);
  menu.SchildArray[SHLD_LOGO]->SetzeSignal(S_BACK);
  menu.SchildArray[SHLD_LOGO]->VollSichtbar();

  menu.TextfeldArray[GetTextOffset()]->Positioniere(8,10.5,1.0,A_MITTE);
  menu.TextfeldArray[GetTextOffset()]->VollSichtbar();

  float y = 9.5;
                menu.TextfeldArray[GetTextOffset() + 2]->Positioniere(1.25,y,0.5,A_LINKS);
  y -= 0.5; if (menu.TextfeldArray.find(GetTextOffset() + 3) != menu.TextfeldArray.end())
                menu.TextfeldArray[GetTextOffset() + 3]->Positioniere(1.25,y,0.5,A_LINKS);
  y -= 0.5; if (menu.TextfeldArray.find(GetTextOffset() + 4) != menu.TextfeldArray.end())
                menu.TextfeldArray[GetTextOffset() + 4]->Positioniere(1.25,y,0.5,A_LINKS);
  y -= 0.5; if (menu.TextfeldArray.find(GetTextOffset() + 5) != menu.TextfeldArray.end())
                menu.TextfeldArray[GetTextOffset() + 5]->Positioniere(1.25,y,0.5,A_LINKS);
  y -= 0.5; if (menu.TextfeldArray.find(GetTextOffset() + 6) != menu.TextfeldArray.end())
                menu.TextfeldArray[GetTextOffset() + 6]->Positioniere(1.25,y,0.5,A_LINKS);
  y -= 0.5; if (menu.TextfeldArray.find(GetTextOffset() + 7) != menu.TextfeldArray.end())
                menu.TextfeldArray[GetTextOffset() + 7]->Positioniere(1.25,y,0.5,A_LINKS);
  y -= 0.5;

  switch (tutorialState)
  {
    case KP_TUT_RIGHT_TOKEN:
                // Klicked (and evtl. moved) the right token
                // Tutorial can be finished
                menu.TextfeldArray[T_WEITER]->Positioniere(8,1,1,A_MITTE);
                menu.TextfeldArray[T_WEITER]->SetzeSignal(S_CONTINUE);
                menu.TextfeldArray[GetTextOffset()+1 ]->Positioniere(1.25,y,0.5,A_LINKS); y -= 0.5;
                break;
    case KP_TUT_NO_TOKEN:
                // Missed any token
                menu.TextfeldArray[T_TUTORIAL34_EMPTY]->Positioniere(1.25,y,0.5,A_LINKS); y -= 0.5;
                menu.TextfeldArray[T_TUTORIAL34_AGAIN]->Positioniere(1.25,y,0.5,A_LINKS); y -= 0.5;
                break;
    case KP_TUT_NOMOV_TOKEN:
                // Klicked on an unmovable token
                menu.TextfeldArray[T_TUTORIAL34_NOMV ]->Positioniere(1.25,y,0.5,A_LINKS); y -= 0.5;
                menu.TextfeldArray[T_TUTORIAL34_AGAIN]->Positioniere(1.25,y,0.5,A_LINKS); y -= 0.5;
                break;
    case KP_TUT_WRONG_TOKEN:
                // Klicked on the wrong token
                menu.TextfeldArray[T_TUTORIAL34_GOOD ]->Positioniere(1.25,y,0.5,A_LINKS); y -= 0.5;
                menu.TextfeldArray[T_TUTORIAL34_AGAIN]->Positioniere(1.25,y,0.5,A_LINKS); y -= 0.5;
                break;
    case KP_TUT_KLICKED_ONLY:
                // Klicked right token, but did not move with the mouse
                // This can only happen in Tutorial 4
                menu.TextfeldArray[T_TUTORIAL4_MOVE  ]->Positioniere(1.25,y,0.5,A_LINKS); y -= 0.5;
                menu.TextfeldArray[T_TUTORIAL34_AGAIN]->Positioniere(1.25,y,0.5,A_LINKS);
                break;
    case KP_TUT_STARTED:
    default:    break;
  }

  StartAnimation();
}

void  KPstateTutorial3::MouseClick (KPstateContext *pContext, int button, int state, int x, int y)
{
  int Signal = KPstate::EvaluateMouseClick(pContext, button, state, x, y);

  switch (Signal) {
    case S_CONTINUE: ContinueWithNextState(pContext);
                     return;
    case S_BACK:     pContext->ChangeState(ESCKeyAction(pContext));
                     return;
  }

  if (tutorialState != KP_TUT_RIGHT_TOKEN)
    MouseMoveToken(pContext, button, state, x, y);
}

void  KPstateTutorial3::KeyPressed (KPstateContext *pContext, unsigned char key, int x, int y)
{
  CHECK_DEFAULT_KEY_PRESSED(pContext, key, x, y);

  switch (key)
  {
    case ' ':
    case 0x0d: ContinueWithNextState(pContext, tutorialState == KP_TUT_RIGHT_TOKEN);
               return;
  }
}

tKPMenuState KPstateTutorial3::ESCKeyAction (KPstateContext *pContext)
{
  pContext->GetCamera().SetzeRundflug(true);
  pContext->GetBoardView().ResetBoard();
  pContext->GetBoardView().EmphasizeToken(TK_EMPTY);
  return (tKPMenuState)pContext->GetMenu().RestoreLastStateId();
}

void KPstateTutorial3::ContinueWithNextState(KPstateContext *pContext, bool valid /* = true */)
{
  if (!valid)
    return;

  pContext->GetCamera().SetzeRundflug(true);
  pContext->GetBoardView().ResetBoard();
  pContext->GetBoardView().EmphasizeToken( TK_EMPTY );
  pContext->ChangeState( KPState_Tutorial4 );
}
  
/////////////////////////////////////////////////////////////////////
// Check what has happened directly after
// Klicked on and evtl. moved a token (either successfully or not)
/////////////////////////////////////////////////////////////////////

void KPstateTutorial3::HookAfterTokenMoved(KPstateContext *pContext, tKPTokenID id,
                           tKPDirection direction, bool successfullyMoved)
{
  tutorialState = KP_TUT_STARTED;

  if (!successfullyMoved && id == TK_EMPTY)
    tutorialState = KP_TUT_NO_TOKEN;
  else if (!successfullyMoved && id == GetEmphasizedTokenId() && direction == MOVE_NO)
    tutorialState = KP_TUT_KLICKED_ONLY;
  else if (!successfullyMoved)
    tutorialState = KP_TUT_NOMOV_TOKEN;
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
  KPnode n;

  n.InitializeToken(TK_GREEN1, "green", 1, 4, 1, 1);
  n.InitializeToken(TK_GREEN2, "green", 0, 4, 1, 1);
  n.InitializeToken(TK_GREEN3, "green", 3, 4, 1, 1);
  n.InitializeToken(TK_GREEN4, "green", 1, 3, 1, 1);
  n.InitializeToken(TK_WHITE1, "white", 0, 0, 1, 2);
  n.InitializeToken(TK_WHITE2, "white", 3, 0, 1, 2);
  n.InitializeToken(TK_WHITE3, "white", 0, 2, 1, 2);
  n.InitializeToken(TK_WHITE4, "white", 3, 2, 1, 2);
  n.InitializeToken(TK_WHITE5, "white", 1, 2, 2, 1);
  n.InitializeToken(TK_RED1,   "red",   1, 0, 2, 2);

  pContext->GetBoardView().SetBoard(&n);
}

void KPstateTutorial3::PlayAudioForInitialize(KPstateContext *pContext)
{
  pContext->GetUserInterface().PlayAudio(KP_SND_TUTORIALNEXT);
}


