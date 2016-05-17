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
    tf_index(Lbl::Solved11)
{
}

void KPstateGameSolved::Initialize(KPstateContext *pContext,
                                   StateId previousStateId)
{
    KPstate::Initialize(pContext, previousStateId);

    auto &menu = pContext->GetMenu();
    auto &statistics = pContext->GetStatistics();
    KPscore scoreList;

    pContext->GetConfig().SavedGame = 0;

    auto it = menu.labels.find(Lbl::NameLabel);

    if (it == menu.labels.end())
    {
        menu.labels[Lbl::NameLabel] = Label();
    }

    menu.labels[Lbl::NameLabel].SetTextOrFormat("");

    if (statistics.Get(EventCounter::MovesWithCheat) != 0)
    {
        // Game solved but cheats used
        tf_index = Lbl::SolvedWithCheat31;
    }
    else if (statistics.Get(EventCounter::MovesWithHint) != 0)
    {
        // Game solved with solution hint
        tf_index = Lbl::SolvedWithHint21;
    }
    else
    {
        // Game solved without any help
        tf_index = Lbl::Solved11;

        if (scoreList.CanAdd("", statistics.GetTotalTime(),
                             statistics.Get(EventCounter::Moves)))
        {
            addToScoreList = true;
        }
    }

    UpdateDisplay(pContext);
}

void KPstateGameSolved::UpdateDisplay(KPstateContext *pContext) const
{
    KPstate::UpdateDisplay(pContext);

    auto &menu = pContext->GetMenu();
    auto &statistics = pContext->GetStatistics();

    menu.plates[KPPlate::MenuBackground].SetPosition(1, 3.0, 15, 8.5);

    menu.plates[KPPlate::Logo].SetPosition(5, 9, 11, 11);
    menu.plates[KPPlate::Logo].SetFullyVisible();

    auto y = 7.5f;
    std::string playTime = statistics.GetTotalTime(TimeFormat::HHMMSS);
    menu.labels[Lbl::Solved].FormatText(
        statistics.Get(EventCounter::Moves),
        playTime);
    menu.labels[Lbl::Solved].SetPosition(8, y, 0.6f, AlignItem::Centered);
    y -= 0.6f;

    auto it = menu.labels.find(tf_index);

    if (it != menu.labels.end())
    {
        menu.labels[tf_index].FormatText(
            statistics.Get(EventCounter::MovesWithHint));
        menu.labels[tf_index].SetPosition(8, y, 0.6f, AlignItem::Centered);
        y -= 0.6f;
    }

    auto labelId = GetLabelId(tf_index, 1);

    it = menu.labels.find(labelId);

    if (it != menu.labels.end())
    {
        menu.labels[labelId].SetPosition(8, y, 0.6f, AlignItem::Centered);
    }

    y -= 0.6f;
    labelId = GetLabelId(tf_index, 2);
    it = menu.labels.find(labelId);

    if (it != menu.labels.end())
    {
        menu.labels[labelId].SetPosition(8, y, 0.6f, AlignItem::Centered);
    }

    y -= 1.0f;

    if (addToScoreList)
    {
        menu.labels[Lbl::InputName].SetPosition(8, y, 1, AlignItem::Centered);
        y -= 1.0f;
        menu.labels[Lbl::NameLabel].SetPosition(8, y, 1, AlignItem::Centered);
        menu.labels[Lbl::NameLabel].SetMaxCharacters(24);
        menu.labels[Lbl::NameLabel].SetInputFocus(addToScoreList);
    }

    menu.labels[Lbl::Continue].SetPosition(8, 1, 1, AlignItem::Centered);
    menu.labels[Lbl::Continue].SetSignal(S_CONTINUE);

    StartAnimation(pContext);
}

void KPstateGameSolved::KeyPressed(KPstateContext *pContext, unsigned char key,
                                   int x, int y) const
{
    if (EvaluateKeyPressed(pContext, key, x, y))
    {
        if (pContext->GetMenu().labels[Lbl::NameLabel].HasInputFocus())
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
        KPstate::KeyPressed(pContext, key, x, y);
    }
}

StateId KPstateGameSolved::ESCKeyAction(KPstateContext *pContext) const
{
    pContext->GetCamera().SetRoundtrip(true);
    return StateId::Finish;
}

void KPstateGameSolved::MouseClick(KPstateContext *pContext,
                                   MouseButton button, MouseButtonEvent event,
                                   int x, int y)
{
    auto Signal = KPstate::EvaluateMouseClick(pContext, button, event, x, y);

    switch (Signal)
    {
        case S_CONTINUE:
            SaveScoreAndChangeState(pContext);
            break;
    }
}

void KPstateGameSolved::SaveScoreAndChangeState(KPstateContext *pContext) const
{
    auto newState = StateId::MainMenu;
    auto name(pContext->GetMenu().labels[Lbl::NameLabel].GetText());

    if (addToScoreList && !name.empty())
    {
        KPscore scoreList;

        scoreList.Add(name.c_str(),
                      pContext->GetStatistics().GetTotalTime(),
                      pContext->GetStatistics().Get(EventCounter::Moves), 0);
        scoreList.WriteToFile();
        pContext->GetCamera().SetRoundtrip(true);
        newState = StateId::ScoreList;
    }

    pContext->ChangeState(newState);
}

void KPstateGameSolved::PlayAudioForInitialize(KPstateContext *pContext) const
{
    pContext->GetUserInterface().PlayAudio(KPSound::GameSolved);
}
