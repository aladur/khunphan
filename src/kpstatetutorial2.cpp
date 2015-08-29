/*
    kpstatetutorial2.cpp


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

#include "kpstatetutorial2.h"
#include "kpmenu.h"
#include "kpboardGL.h"
#include "Kamera.h"
#include "kpuibase.h"


void KPstateTutorial2::Initialize(KPstateContext *pContext,  const KPstate *pOldState)
{
  KPstate::Initialize(pContext, pOldState);

  // Do some initialization stuff here:
  pContext->GetBoardView().EmphasizeToken(TK_RED1);
  pContext->GetCamera().SetzeRundflug(false);
  InitializeBoardWithTokens(pContext);

  UpdateDisplay(pContext);
}

void KPstateTutorial2::UpdateDisplay(KPstateContext *pContext)
{
  KPstate::UpdateDisplay(pContext);

  KPmenu &menu = pContext->GetMenu();

  menu.SchildArray[SHLD_MENUBACKGROUND]->Positioniere(.5,4.5,15.5,11);

  menu.SchildArray[SHLD_LOGO]->Positioniere(0,11.0,4,12);
  menu.SchildArray[SHLD_LOGO]->SetzeSignal(S_BACK);
  menu.SchildArray[SHLD_LOGO]->VollSichtbar();

  menu.TextfeldArray[T_TUTORIAL2]->Positioniere(8,10.5,1.0,A_MITTE);
  menu.TextfeldArray[T_TUTORIAL2]->VollSichtbar();

  float y = 9.5;
                menu.TextfeldArray[T_TUTORIAL2_01]->Positioniere(1.25,y,0.5,A_LINKS);
  y -= 0.5; if (menu.TextfeldArray.find(T_TUTORIAL2_02) != menu.TextfeldArray.end())
                menu.TextfeldArray[T_TUTORIAL2_02]->Positioniere(1.25,y,0.5,A_LINKS);
  y -= 0.5; if (menu.TextfeldArray.find(T_TUTORIAL2_03) != menu.TextfeldArray.end())
                menu.TextfeldArray[T_TUTORIAL2_03]->Positioniere(1.25,y,0.5,A_LINKS);
  y -= 0.5; if (menu.TextfeldArray.find(T_TUTORIAL2_04) != menu.TextfeldArray.end())
                menu.TextfeldArray[T_TUTORIAL2_04]->Positioniere(1.25,y,0.5,A_LINKS);
  y -= 0.5; if (menu.TextfeldArray.find(T_TUTORIAL2_05) != menu.TextfeldArray.end())
                menu.TextfeldArray[T_TUTORIAL2_05]->Positioniere(1.25,y,0.5,A_LINKS);

  menu.TextfeldArray[T_WEITER]->Positioniere(8,1,1,A_MITTE);
  menu.TextfeldArray[T_WEITER]->SetzeSignal(S_CONTINUE);

  StartAnimation();
}

void KPstateTutorial2::KeyPressed (KPstateContext *pContext, unsigned char key, int x, int y)
{
  CHECK_DEFAULT_KEY_PRESSED(pContext, key, x, y);

  switch (key)
  {
    case ' ':
    case 0x0d: pContext->ChangeState(KPState_Tutorial3); break;
  }
}

tKPMenuState KPstateTutorial2::ESCKeyAction (KPstateContext *pContext)
{
  pContext->GetCamera().SetzeRundflug(true);
  pContext->GetBoardView().EmphasizeToken(TK_EMPTY);
  return (tKPMenuState)pContext->GetMenu().RestoreLastStateId();
}

void  KPstateTutorial2::MouseClick (KPstateContext *pContext, int button, int state, int x, int y)
{
  int Signal = KPstate::EvaluateMouseClick(pContext, button, state, x, y);

  switch (Signal) {
    case S_CONTINUE: pContext->ChangeState(KPState_Tutorial3);      break;
    case S_BACK:     pContext->ChangeState(ESCKeyAction(pContext)); break;
  }
}

void KPstateTutorial2::InitializeBoardWithTokens(KPstateContext *pContext)
{
  KPnode n;

  n.InitializeToken(TK_GREEN1, "green", 0, 0, 1, 1);
  n.InitializeToken(TK_GREEN2, "green", 3, 3, 1, 1);
  n.InitializeToken(TK_GREEN3, "green", 3, 4, 1, 1);
  n.InitializeToken(TK_GREEN4, "green", 1, 2, 1, 1);
  n.InitializeToken(TK_WHITE1, "white", 0, 1, 1, 2);
  n.InitializeToken(TK_WHITE2, "white", 1, 0, 1, 2);
  n.InitializeToken(TK_WHITE3, "white", 2, 0, 1, 2);
  n.InitializeToken(TK_WHITE4, "white", 3, 0, 1, 2);
  n.InitializeToken(TK_WHITE5, "white", 2, 2, 2, 1);
  n.InitializeToken(TK_RED1,   "red",   1, 3, 2, 2);

  pContext->GetBoardView().SetBoard(&n);
}

void KPstateTutorial2::PlayAudioForInitialize(KPstateContext *pContext)
{
  pContext->GetUserInterface().PlayAudio(KP_SND_TUTORIALNEXT);
}
