/*
    kpstategamesolved.cpp

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
#include "kpstategamesolved.h"
#include "kpmenu.h"
#include "kpconfig.h"
#include "kpboardGL.h"
#include "camera.h"
#include "kpstatistics.h"
#include "kpscore.h"
#include "kpuibase.h"


KPstateGameSolved::KPstateGameSolved() : addToScoreList(false),
    tf_index(T_SOLVED2_1)
{
}

void KPstateGameSolved::Initialize(KPstateContext *pContext,
                                   const KPstate *pPreviousState)
{
    tIdToLabel::const_iterator it;

    KPstate::Initialize(pContext, pPreviousState);

    KPmenu &menu             = pContext->GetMenu();
    KPStatistics &statistics = pContext->GetStatistics();

    // Do some initialization stuff here:
    KPscore scoreList;

    KPConfig::Instance().SavedGame = 0;

    it = menu.labels.find(T_NAME_LABEL);
    if (it == menu.labels.end())
    {
        menu.labels[T_NAME_LABEL] = Label();
    }
    menu.labels[T_NAME_LABEL].SetTextOrFormat("");

    if (statistics.GetEventCounter(USED_CHEATS_CNT) != 0)
    {
        // Game solved but cheats used
        tf_index = T_SOLVED2_3;
    }
    else if (statistics.GetEventCounter(MOVE_WITH_HELP_CNT) != 0)
    {
        // Game solved with solution hint
        tf_index = T_SOLVED2_2;
    }
    else
    {
        // Game solved without any help
        tf_index = T_SOLVED2_1;
        if (scoreList.CanAdd("", statistics.GetTotalTime(),
                                      statistics.GetEventCounter(MOVE_COUNTER)))
        {
            addToScoreList = true;
        }
    }

    UpdateDisplay(pContext);
}

void KPstateGameSolved::UpdateDisplay(KPstateContext *pContext) const
{
    tIdToLabel::const_iterator it;

    KPstate::UpdateDisplay(pContext);

    KPmenu &menu             = pContext->GetMenu();
    KPStatistics &statistics = pContext->GetStatistics();

    menu.plates[PLATE_MENUBACKGROUND].SetPosition(1,3.0,15,8.5);

    menu.plates[PLATE_LOGO].SetPosition(5,9,11,11);
    menu.plates[PLATE_LOGO].SetFullyVisible();

    float y = 7.5;
    menu.labels[T_SOLVED1].FormatText(2,
            statistics.GetEventCounter(MOVE_COUNTER),
            statistics.GetTotalTime(RTIME_MM_ss).c_str());
    menu.labels[T_SOLVED1].SetPosition(8,y,0.6f,A_CENTERED);
    y-= 0.6f;

    it = menu.labels.find(tf_index);
    if (it != menu.labels.end())
    {
        menu.labels[tf_index].FormatText(
                1, statistics.GetEventCounter(MOVE_WITH_HELP_CNT));
        menu.labels[tf_index].SetPosition(8,y,0.6f,A_CENTERED);
        y-= 0.6f;
    }

    it = menu.labels.find(tf_index+1);
    if (it != menu.labels.end())
    {
        menu.labels[tf_index+1].SetPosition(8,y,0.6f,A_CENTERED);
    }

    y-= 0.6f;
    it = menu.labels.find(tf_index+2);
    if (it != menu.labels.end())
    {
        menu.labels[tf_index+2].SetPosition(8,y,0.6f,A_CENTERED);
    }
    y-= 1.0;

    if (addToScoreList)
    {
        menu.labels[T_INPUT_NAME].SetPosition(8,y,1,A_CENTERED);
        y -= 1.0;
        menu.labels[T_NAME_LABEL].SetPosition(8,y,1, A_CENTERED);
        menu.labels[T_NAME_LABEL].SetMaxCharacters(24);
        menu.labels[T_NAME_LABEL].SetInputFocus(addToScoreList);
    }

    menu.labels[T_CONTINUE].SetPosition(8,1,1,A_CENTERED);
    menu.labels[T_CONTINUE].SetSignal(S_CONTINUE);

    StartAnimation(pContext);
}

void KPstateGameSolved::KeyPressed (KPstateContext *pContext, unsigned char key,
                                    int x, int y) const
{
    if (EvaluateKeyPressed(pContext, key, x, y))
    {
        if (pContext->GetMenu().labels[T_NAME_LABEL].HasInputFocus())
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

tKPMenuState KPstateGameSolved::ESCKeyAction(KPstateContext *pContext) const
{
    pContext->GetCamera().SetRoundtrip(true);
    return KPState_Finish;
}

void KPstateGameSolved::MouseClick (KPstateContext *pContext,
                                    tMouseButton button, tMouseEvent event,
                                    int x, int y)
{
    int Signal = KPstate::EvaluateMouseClick(pContext, button, event, x, y);

    switch (Signal)
    {
        case S_CONTINUE:
            SaveScoreAndChangeState(pContext);
            break;
    }
}

void KPstateGameSolved::SaveScoreAndChangeState(KPstateContext *pContext) const
{
    tKPMenuState newState = KPState_MainMenu;
    std::string name(pContext->GetMenu().labels[T_NAME_LABEL].GetText());

    if (addToScoreList && !name.empty())
    {
        KPscore scoreList;

        scoreList.Add(name.c_str(),
                      pContext->GetStatistics().GetTotalTime(),
                      pContext->GetStatistics()
                        .GetEventCounter(MOVE_COUNTER), 0);
        scoreList.WriteToFile();
        pContext->GetCamera().SetRoundtrip(true);
        newState = KPState_ScoreList;
    }
    pContext->ChangeState(newState);
}

void KPstateGameSolved::PlayAudioForInitialize(KPstateContext *pContext) const
{
    pContext->GetUserInterface().PlayAudio(KP_SND_GAMESOLVED);
}
