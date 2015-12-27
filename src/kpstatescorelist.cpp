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


void KPstateScoreList::Initialize(KPstateContext *pContext,
                                  const KPstate *pOldState)
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
    std::string  Name;
    unsigned int PlayTime;
    unsigned int Moves;

    KPscore::Instance().ReadFromFile();
    max = KPscore::Instance().GetEntryCount();
    if (max > 10)
    {
        max = 10;
    }

    menu.plates[PLATE_MENUBACKGROUND].SetPosition(1,2.0,15,9);

    menu.plates[PLATE_LOGO].SetPosition(5,9,11,11);
    menu.plates[PLATE_LOGO].SetFullyVisible();

    menu.labels[T_SCORELIST].SetPosition(8, 8.5, 1, A_CENTERED);
    menu.labels[T_SCORELIST].SetFullyVisible();

    float y = 7.5;
    menu.labels[T_S_NAME].SetPosition(2, y, 0.7f);
    menu.labels[T_S_PLAYTIME].SetPosition(8, y, 0.7f);
    menu.labels[T_S_PLAYTIME].SetSignal(S_PLAYTIME);
    menu.labels[T_S_MOVES].SetPosition(11,y, 0.7f);
    menu.labels[T_S_MOVES].SetSignal(S_MOVES);

    y -= 0.7f;
    for (i = 0; i < max; i++)
    {
        tIdToLabel::const_iterator it;

        it = menu.labels.find(T_S_NAME1 + i);
        if (it == menu.labels.end())
        {
            menu.labels[T_S_NAME1 + i] = Label("%s");
        }

        it = menu.labels.find(T_S_PLAYTIME1 + i);
        if (it == menu.labels.end())
        {
            menu.labels[T_S_PLAYTIME1 + i] = Label("%d:%02d:%02d");
        }

        it = menu.labels.find(T_S_MOVES1 + i);
        if (it == menu.labels.end())
        {
            menu.labels[T_S_MOVES1 + i] = Label("%u");
        }

        KPscore::Instance().Get(i, Name, &PlayTime, &Moves);

        menu.labels[T_S_NAME1 + i].FormatText(1, Name.c_str());
        menu.labels[T_S_PLAYTIME1 + i].FormatText(3,
                PlayTime / 3600000,
                (PlayTime % 3600000) / 60000,
                (PlayTime % 60000) / 1000 );
        menu.labels[T_S_MOVES1 + i].FormatText(1, Moves);


        menu.labels[T_S_NAME1 + i].SetPosition(2, y, 0.6f);
        menu.labels[T_S_PLAYTIME1 + i].SetPosition(9, y, 0.6f, A_CENTERED);
        menu.labels[T_S_MOVES1 + i].SetPosition(12,y, 0.6f, A_CENTERED);

        y -= 0.5f;
    }

    KPscore::Instance().CheckPlayTime(true);

    menu.labels[T_CONTINUE].SetPosition(8,1,1,A_CENTERED);
    menu.labels[T_CONTINUE].SetSignal(S_CONTINUE);

    StartAnimation();
}

void KPstateScoreList::KeyPressed (KPstateContext *pContext, unsigned char key,
                                   int x, int y)
{
    CHECK_DEFAULT_KEY_PRESSED(pContext, key, x, y);
}

void  KPstateScoreList::MouseClick (KPstateContext *pContext,
                                    tMouseButton button, tMouseEvent event,
                                    int x, int y)
{
    int Signal = KPstate::EvaluateMouseClick(pContext, button, event, x, y);

    switch (Signal)
    {
        case S_CONTINUE:
            pContext->ChangeState(KPState_MainMenu);
            return;
        case S_PLAYTIME:
            KPscore::Instance().CheckPlayTime(true);
            UpdateDisplay(pContext);
            break;
        case S_MOVES:
            KPscore::Instance().CheckPlayTime(false);
            UpdateDisplay(pContext);
            break;
    }
}

tKPMenuState KPstateScoreList::ESCKeyAction (KPstateContext *pContext)
{
    return pContext->GetMenu().RestoreLastStateId();
}
