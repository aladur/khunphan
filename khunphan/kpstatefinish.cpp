/*
    kpstatefinish.h

    Automatic solution finder for KhunPhan game
    Copyright (C) 2001,2002,2003  W. Schwotzer

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

#include "kpstatefinish.h"
#include "kpmenu.h"
#include "kpuibase.h"


void KPstateFinish::Initialize(KPstateContext *pContext,  const KPstate *pOldState)
{
  KPstate::Initialize(pContext, pOldState);

  // Do some initialization stuff here:

  UpdateDisplay(pContext);
}

void KPstateFinish::UpdateDisplay(KPstateContext *pContext)
{
  KPstate::UpdateDisplay(pContext);

  KPmenu &menu = pContext->GetMenu();

  menu.SchildArray[SHLD_MENUBACKGROUND]->Positioniere(4.5,4.5,11.5,8.5);

  menu.SchildArray[SHLD_LOGO]->Positioniere(4,9,12,11);
  menu.SchildArray[SHLD_LOGO]->VollSichtbar();

  menu.TextfeldArray[T_BEENDEN  ]->Positioniere(8,8,1,A_MITTE);
  menu.TextfeldArray[T_BEENDEN  ]->VollSichtbar();
  menu.TextfeldArray[T_JABEENDEN]->Positioniere(8,6,1,A_MITTE);
  menu.TextfeldArray[T_JABEENDEN]->SetzeSignal(S_FINISH);
  menu.TextfeldArray[T_ZURUECK  ]->Positioniere(8,1,1,A_MITTE);
  menu.TextfeldArray[T_ZURUECK  ]->SetzeSignal(S_BACK);

  StartAnimation();
}

void KPstateFinish::KeyPressed (KPstateContext *pContext, unsigned char key, int x, int y)
{
  CHECK_DEFAULT_KEY_PRESSED(pContext, key, x, y);
}

void  KPstateFinish::MouseClick (KPstateContext *pContext, int button, int state, int x, int y)
{
  int Signal = KPstate::EvaluateMouseClick(pContext, button, state, x, y);

  switch (Signal) {
    case S_FINISH: pContext->ChangeState(KPState_Shutdown); break;
    case S_BACK:   pContext->ChangeState(oldStateId);       break;
  }
}

void KPstateFinish::PlayAudioForInitialize(KPstateContext *pContext)
{
  pContext->GetUserInterface().PlayAudio(KP_SND_QUITGAME);
}
