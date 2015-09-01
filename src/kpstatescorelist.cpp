/*
    kpstatescorelist.cpp

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

#include "kpstatescorelist.h"
#include "kpscore.h"
#include "kpmenu.h"


void KPstateScoreList::Initialize(KPstateContext *pContext,  const KPstate *pOldState)
{
  KPstate::Initialize(pContext, pOldState);

  // Do some initialization stuff here:

  UpdateDisplay(pContext);
}

void KPstateScoreList::UpdateDisplay(KPstateContext *pContext)
{
  KPstate::UpdateDisplay(pContext);

  KPmenu &menu = pContext->GetMenu();

  int          i, max;
  BString      Name;
  unsigned int PlayTime;
  unsigned int Moves;

  KPscore::Instance().ReadFromFile();
  max = KPscore::Instance().GetEntryCount();
  if (max > 10) max = 10;
  
  menu.SchildArray[SHLD_MENUBACKGROUND]->Positioniere(1,2.0,15,9);

  menu.SchildArray[SHLD_LOGO]->Positioniere(5,9,11,11);
  menu.SchildArray[SHLD_LOGO]->VollSichtbar();

  menu.TextfeldArray[T_SCORELIST ]->Positioniere(8, 8.5, 1, A_MITTE);
  menu.TextfeldArray[T_SCORELIST ]->VollSichtbar();

  float y = 7.5;
  menu.TextfeldArray[T_S_NAME    ]->Positioniere(2, y, 0.7f, A_LINKS);
  menu.TextfeldArray[T_S_PLAYTIME]->Positioniere(8, y, 0.7f, A_LINKS);
  menu.TextfeldArray[T_S_PLAYTIME]->SetzeSignal(S_PLAYTIME);
  menu.TextfeldArray[T_S_MOVES   ]->Positioniere(11,y, 0.7f, A_LINKS);
  menu.TextfeldArray[T_S_MOVES   ]->SetzeSignal(S_MOVES);

  y -= 0.7f;
  for (i = 0; i < max; i++)
  {
    if (menu.TextfeldArray[T_S_NAME1 + i] == NULL)
    {
      menu.TextfeldArray[T_S_NAME1   + i] = new Textfeld();
      menu.TextfeldArray[T_S_NAME1   + i]->Initialisiere("");
    }
    if (menu.TextfeldArray[T_S_PLAYTIME1 + i] == NULL)
    {
      menu.TextfeldArray[T_S_PLAYTIME1   + i] = new Textfeld();
      menu.TextfeldArray[T_S_PLAYTIME1   + i]->Initialisiere("");
    }
    if (menu.TextfeldArray[T_S_MOVES1 + i] == NULL)
    {
      menu.TextfeldArray[T_S_MOVES1   + i] = new Textfeld();
      menu.TextfeldArray[T_S_MOVES1   + i]->Initialisiere("");
    }

    KPscore::Instance().Get(i, Name, &PlayTime, &Moves);

    menu.TextfeldArray[T_S_NAME1     + i]->FormatText("%s", Name.c_str());
    menu.TextfeldArray[T_S_PLAYTIME1 + i]->FormatText("%d:%02d:%02d",
      PlayTime / 3600000, (PlayTime % 3600000) / 60000, (PlayTime % 60000) / 1000 );
    menu.TextfeldArray[T_S_MOVES1    + i]->FormatText("%u", Moves);


    menu.TextfeldArray[T_S_NAME1     + i]->Positioniere(2, y, 0.6f, A_LINKS);
    menu.TextfeldArray[T_S_PLAYTIME1 + i]->Positioniere(9, y, 0.6f, A_MITTE);
    menu.TextfeldArray[T_S_MOVES1    + i]->Positioniere(12,y, 0.6f, A_MITTE);

    y -= 0.5f;
  }
  KPscore::Instance().CheckPlayTime(true);
  
  menu.TextfeldArray[T_WEITER]->Positioniere(8,1,1,A_MITTE);
  menu.TextfeldArray[T_WEITER]->SetzeSignal(S_CONTINUE);

  StartAnimation();
}

void KPstateScoreList::KeyPressed (KPstateContext *pContext, unsigned char key, int x, int y)
{
  CHECK_DEFAULT_KEY_PRESSED(pContext, key, x, y);
}

void  KPstateScoreList::MouseClick (KPstateContext *pContext, int button, int state, int x, int y)
{
  int Signal = KPstate::EvaluateMouseClick(pContext, button, state, x, y);

  switch (Signal) {
    case S_CONTINUE: pContext->ChangeState(KPState_MainMenu);  return;
    case S_PLAYTIME: KPscore::Instance().CheckPlayTime(true);
                     UpdateDisplay(pContext); break;
    case S_MOVES:    KPscore::Instance().CheckPlayTime(false);
                     UpdateDisplay(pContext); break;
  }
}

tKPMenuState KPstateScoreList::ESCKeyAction (KPstateContext *pContext)
{
  return (tKPMenuState)pContext->GetMenu().RestoreLastStateId();
}
