/*
    kpstatescorelist.cpp

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
#include "kpstatescorelist.h"
#include "kpscore.h"
#include "kpstatistics.h"
#include "kpmenu.h"


void KPstateScoreList::Initialize(KPstateContext *pContext,
                                  tKPMenuState previousStateId)
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

    menu.plates[PLATE_MENUBACKGROUND].SetPosition(1, 2.0, 15, 9);

    menu.plates[PLATE_LOGO].SetPosition(5, 9, 11, 11);
    menu.plates[PLATE_LOGO].SetFullyVisible();

    menu.labels[T_SCORELIST].SetPosition(8, 8.5, 1, AlignItem::Centered);
    menu.labels[T_SCORELIST].SetFullyVisible();

    auto y = 7.5f;
    menu.labels[T_S_NAME].SetPosition(2, y, 0.7f);
    menu.labels[T_S_PLAYTIME].SetPosition(8, y, 0.7f);
    menu.labels[T_S_MOVES].SetPosition(11, y, 0.7f);

    y -= 0.7f;

    for (decltype(max) i = 0; i < max; i++)
    {
        auto it = menu.labels.find(T_S_NAME1 + i);

        if (it == menu.labels.end())
        {
            menu.labels[T_S_NAME1 + i] = Label("{0}");
        }

        it = menu.labels.find(T_S_PLAYTIME1 + i);

        if (it == menu.labels.end())
        {
            menu.labels[T_S_PLAYTIME1 + i] = Label("{0}");
        }

        it = menu.labels.find(T_S_MOVES1 + i);

        if (it == menu.labels.end())
        {
            menu.labels[T_S_MOVES1 + i] = Label("{0}");
        }

        scoreList.Get(i, Name, &PlayTime, &Moves);
        auto timeString = KPStatistics::FormatTime(RTIME_HHMMSS, PlayTime);
        menu.labels[T_S_NAME1 + i].FormatText(Name);
        menu.labels[T_S_PLAYTIME1 + i].FormatText(timeString);
        menu.labels[T_S_MOVES1 + i].FormatText(Moves);

        menu.labels[T_S_NAME1 + i].SetPosition(2, y, 0.6f);
        menu.labels[T_S_PLAYTIME1 + i].SetPosition(9, y, 0.6f,
                                                   AlignItem::Centered);
        menu.labels[T_S_MOVES1 + i].SetPosition(12, y, 0.6f,
                                                AlignItem::Centered);

        y -= 0.5f;
    }

    menu.labels[T_CONTINUE].SetPosition(8, 1, 1, AlignItem::Centered);
    menu.labels[T_CONTINUE].SetSignal(S_CONTINUE);

    StartAnimation(pContext);
}

void  KPstateScoreList::MouseClick(KPstateContext *pContext,
                                   MouseButton button, MouseButtonEvent event,
                                   int x, int y)
{
    auto Signal = KPstate::EvaluateMouseClick(pContext, button, event, x, y);

    switch (Signal)
    {
        case S_CONTINUE:
            pContext->ChangeState(KPState_MainMenu);
            return;
    }
}

tKPMenuState KPstateScoreList::ESCKeyAction(KPstateContext *pContext) const
{
    return pContext->GetMenu().RestoreLastStateId();
}
