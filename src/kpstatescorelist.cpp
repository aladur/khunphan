/*
    kpstatescorelist.cpp

    Automatic solution finder for KhunPhan game
    Copyright (C) 2001-2018  W. Schwotzer

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
#include "kpstatescorelist.h"
#include "kpscore.h"
#include "kpstatistics.h"
#include "kpmenu.h"


void KPstateScoreList::Initialize(KPstateContext *pContext,
                                  StateId previousStateId)
{
    KPstate::Initialize(pContext, previousStateId);

    UpdateDisplay(pContext);
}

void KPstateScoreList::UpdateDisplay(KPstateContext *pContext) const
{
    KPstate::UpdateDisplay(pContext);

    auto &menu = pContext->GetMenu();

    std::string  Name;
    unsigned int PlayTime;
    unsigned int Moves;
    KPscore scoreList;

    auto max = scoreList.GetEntryCount();

    if (max > 10)
    {
        max = 10;
    }

    menu.plates[KPPlate::MenuBackground].SetPosition(1, 2.0, 15, 9);

    menu.plates[KPPlate::Logo].SetPosition(5, 9, 11, 11);
    menu.plates[KPPlate::Logo].SetFullyVisible();

    menu.labels[Lbl::ScoreList].SetPosition(8, 8.5, 1, AlignItem::Centered);
    menu.labels[Lbl::ScoreList].SetFullyVisible();

    auto y = 7.5f;
    menu.labels[Lbl::ScoreListName].SetPosition(2, y, 0.7f);
    menu.labels[Lbl::ScoreListPlayTime].SetPosition(8, y, 0.7f);
    menu.labels[Lbl::ScoreListMoves].SetPosition(11, y, 0.7f);

    y -= 0.7f;

    for (decltype(max) i = 0; i < max; i++)
    {
        auto labelId = GetLabelId(Lbl::ScoreListName1, i);
        auto it = menu.labels.find(labelId);

        if (it == menu.labels.end())
        {
            menu.labels[labelId] = Label("{0}");
        }

        labelId = GetLabelId(Lbl::ScoreListPlayTime1, i);
        it = menu.labels.find(labelId);

        if (it == menu.labels.end())
        {
            menu.labels[labelId] = Label("{0}");
        }

        labelId = GetLabelId(Lbl::ScoreListMoves1, i);
        it = menu.labels.find(labelId);

        if (it == menu.labels.end())
        {
            menu.labels[labelId] = Label("{0}");
        }

        scoreList.Get(i, Name, &PlayTime, &Moves);
        auto timeString =
            KPStatistics::FormatTime(TimeFormat::HHMMSS, PlayTime);
        menu.labels[GetLabelId(Lbl::ScoreListName1, i)].FormatText(Name);
        menu.labels[GetLabelId(Lbl::ScoreListPlayTime1, i)].FormatText(
            timeString);
        menu.labels[GetLabelId(Lbl::ScoreListMoves1, i)].FormatText(Moves);

        menu.labels[GetLabelId(Lbl::ScoreListName1, i)].SetPosition(2, y, 0.6f);
        menu.labels[GetLabelId(Lbl::ScoreListPlayTime1, i)].SetPosition(
            9, y, 0.6f,
            AlignItem::Centered);
        menu.labels[GetLabelId(Lbl::ScoreListMoves1, i)].SetPosition(
            12, y, 0.6f,
            AlignItem::Centered);

        y -= 0.5f;
    }

    menu.labels[Lbl::Continue].SetPosition(8, 1, 1, AlignItem::Centered);
    menu.labels[Lbl::Continue].SetSignal(Signal::Continue);

    StartAnimation(pContext);
}

void  KPstateScoreList::MouseClick(KPstateContext *pContext,
                                   MouseButton button, MouseButtonEvent event,
                                   int x, int y)
{
    auto signal = KPstate::EvaluateMouseClick(pContext, button, event, x, y);

    switch (signal)
    {
        case Signal::Continue:
            pContext->ChangeState(StateId::MainMenu);
            return;

        default:
            break;
    }
}

StateId KPstateScoreList::ESCKeyAction(KPstateContext *pContext) const
{
    return pContext->GetMenu().RestoreLastStateId();
}
