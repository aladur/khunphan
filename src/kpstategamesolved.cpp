/*
    kpstategamesolved.cpp

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

#include "kpstategamesolved.h"
#include "kpmenu.h"
#include "kpconfig.h"
#include "kpboardGL.h"
#include "Kamera.h"
#include "kpstatistics.h"
#include "kpscore.h"
#include "kpuibase.h"


KPstateGameSolved::KPstateGameSolved() : addToScoreList(false),
    tf_index(T_SOLVED2_1)
{
}

void KPstateGameSolved::Initialize(KPstateContext *pContext,
                                   const KPstate *pOldState)
{
    KPstate::Initialize(pContext, pOldState);

    KPmenu &menu             = pContext->GetMenu();
    KPStatistics &statistics = pContext->GetStatistics();

    // Do some initialization stuff here:
    KPscore::Instance().ReadFromFile();

    KPConfig::Instance().SavedGame = 0;

    if (menu.TextfeldArray[T_NAME_FIELD] == NULL)
    {
        menu.TextfeldArray[T_NAME_FIELD] = new Textfeld();
    }
    menu.TextfeldArray[T_NAME_FIELD]->Initialisiere("");

    if (statistics.GetEventCounter(USED_CHEATS_CNT) != 0)
        // Game solved but cheats used
    {
        tf_index = T_SOLVED2_3;
    }
    else if (statistics.GetEventCounter(MOVE_WITH_HELP_CNT) != 0)
        // Game solved with solution hint
    {
        tf_index = T_SOLVED2_2;
    }
    else
    {
        // Game solved without any help
        tf_index = T_SOLVED2_1;
        if(KPscore::Instance().CanAdd("", statistics.GetTotalTime(),
                                      statistics.GetEventCounter(MOVE_COUNTER)))
        {
            addToScoreList = true;
        }
    }

    UpdateDisplay(pContext);
}

void KPstateGameSolved::UpdateDisplay(KPstateContext *pContext)
{
    KPstate::UpdateDisplay(pContext);

    KPmenu &menu             = pContext->GetMenu();
    KPStatistics &statistics = pContext->GetStatistics();

    menu.SchildArray[SHLD_MENUBACKGROUND]->Positioniere(1,3.0,15,8.5);

    menu.SchildArray[SHLD_LOGO]->Positioniere(5,9,11,11);
    menu.SchildArray[SHLD_LOGO]->VollSichtbar();

    float y = 7.5;
    menu.TextfeldArray[T_SOLVED1]->FormatText(NULL,
            statistics.GetEventCounter(MOVE_COUNTER),
            statistics.GetTotalTime(RTIME_MM_ss).c_str());
    menu.TextfeldArray[T_SOLVED1]->Positioniere(8,y,0.6f,A_MITTE);
    y-= 0.6f;

    if (menu.TextfeldArray[tf_index  ] != NULL)
    {
        menu.TextfeldArray[tf_index  ]->FormatText(NULL,
                statistics.GetEventCounter(MOVE_WITH_HELP_CNT));
        menu.TextfeldArray[tf_index  ]->Positioniere(8,y,0.6f,A_MITTE);
        y-= 0.6f;
    }
    if (menu.TextfeldArray[tf_index+1] != NULL)
    {
        menu.TextfeldArray[tf_index+1]->Positioniere(8,y,0.6f,A_MITTE);
    }
    y-= 0.6f;
    if (menu.TextfeldArray[tf_index+2] != NULL)
    {
        menu.TextfeldArray[tf_index+2]->Positioniere(8,y,0.6f,A_MITTE);
    }
    y-= 1.0;

    if (addToScoreList)
    {
        menu.TextfeldArray[T_INPUT_NAME]->Positioniere(8,y,1,A_MITTE);
        y -= 1.0;
        menu.TextfeldArray[T_NAME_FIELD]->Positioniere(8,y,1, A_MITTE);
        menu.TextfeldArray[T_NAME_FIELD]->SetMaxCharacters(24);
        menu.TextfeldArray[T_NAME_FIELD]->SetInputFocus(addToScoreList);
    }

    menu.TextfeldArray[T_WEITER]->Positioniere(8,1,1,A_MITTE);
    menu.TextfeldArray[T_WEITER]->SetzeSignal(S_CONTINUE);

    StartAnimation();
}

void KPstateGameSolved::KeyPressed (KPstateContext *pContext, unsigned char key,
                                    int x, int y)
{
    if (EvaluateKeyPressed(pContext, key, x, y))
    {
        if (pContext->GetMenu().TextfeldArray[T_NAME_FIELD]->HasInputFocus())
        {
            UpdateDisplay(pContext);
        }
        else
        {
            SaveScoreAndChangeState(pContext);
        }
    }
    else
    {
        CHECK_DEFAULT_KEY_PRESSED(pContext, key, x, y);
    }
}

tKPMenuState KPstateGameSolved::ESCKeyAction (KPstateContext *pContext)
{
    pContext->GetCamera().SetzeRundflug(true);
    return KPState_Finish;
}

void KPstateGameSolved::MouseClick (KPstateContext *pContext, int button,
                                    int state, int x, int y)
{
    int Signal = KPstate::EvaluateMouseClick(pContext, button, state, x, y);

    switch (Signal)
    {
        case S_CONTINUE:
            SaveScoreAndChangeState(pContext);
            break;
    }
}

void KPstateGameSolved::SaveScoreAndChangeState(KPstateContext *pContext)
{
    tKPMenuState newState = KPState_MainMenu;

    if (addToScoreList)
    {
        KPscore::Instance().Add(pContext->GetMenu()
                                   .TextfeldArray[T_NAME_FIELD]->Text(),
                                pContext->GetStatistics().GetTotalTime(),
                                pContext->GetStatistics()
                                   .GetEventCounter(MOVE_COUNTER), 0);
        KPscore::Instance().WriteToFile();
        pContext->GetCamera().SetzeRundflug(true);
        newState = KPState_ScoreList;
    }
    pContext->ChangeState(newState);
}
void KPstateGameSolved::PlayAudioForInitialize(KPstateContext *pContext)
{
    pContext->GetUserInterface().PlayAudio(KP_SND_GAMESOLVED);
}
