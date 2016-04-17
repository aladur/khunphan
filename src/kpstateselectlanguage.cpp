/*
    kpstateselectlanguage.cpp

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
#include "kpstateselectlanguage.h"
#include "kpmenu.h"
#include "kpconfig.h"
#include "blogger.h"


void KPstateSelectLanguage::Initialize(KPstateContext *pContext,
                                       const KPstate *pPreviousState)
{
    KPstate::Initialize(pContext, pPreviousState);

    UpdateDisplay(pContext);
}

void KPstateSelectLanguage::UpdateDisplay(KPstateContext *pContext) const
{
    auto &menu = pContext->GetMenu();

    KPstate::UpdateDisplay(pContext);

    menu.plates[PLATE_MENUBACKGROUND].SetPosition(2, 2.5, 14, 8.5);

    menu.plates[PLATE_LOGO].SetPosition(5, 9, 11, 11);
    menu.plates[PLATE_LOGO].SetFullyVisible();

    menu.labels[T_SELECTLANGUAGE].SetPosition(8, 8, 1, A_CENTERED);
    menu.labels[T_SELECTLANGUAGE].SetFullyVisible();

    auto Pos_y = 7.0f;

    for (int i = T_LANGUAGE1; i <= T_LANGUAGE_MAX; ++i)
    {
        if (menu.labels.find(i) != menu.labels.end())
        {
            menu.labels[i].SetPosition(8, Pos_y, 0.71f, A_CENTERED);
            menu.labels[i].SetSignal(i);
            Pos_y -= .5;
        }
    }

    if (pContext->GetConfig().Language)
    {
        menu.labels[T_BACK].SetPosition(8, 1, 1, A_CENTERED);
        menu.labels[T_BACK].SetSignal(S_BACK);
    }

    StartAnimation(pContext);
}

void KPstateSelectLanguage::MouseClick(KPstateContext *pContext,
                                       tMouseButton button, tMouseEvent event,
                                       int x, int y)
{
    auto Signal = KPstate::EvaluateMouseClick(pContext, button, event, x, y);

    switch (Signal)
    {
        case S_BACK:
            pContext->ChangeState(pContext->GetPreviousState());
            break;
    }

    // Language has been changed
    if (Signal >= T_LANGUAGE1 && Signal <= T_LANGUAGE_MAX)
    {
        SetLanguage(pContext, Signal);
        pContext->ChangeState(KPState_MainMenu);
    }
}

tKPMenuState KPstateSelectLanguage::ESCKeyAction(KPstateContext *pContext) const
{
    if (pContext->GetConfig().Language)
    {
        return pContext->GetMenu().RestoreLastStateId();
    }
    else
    {
        // Default: Set Language to "English"
        SetLanguage(pContext, T_LANGUAGE1 + 1);
        return KPState_MainMenu;
    }
}

void KPstateSelectLanguage::SetLanguage(KPstateContext *pContext,
                                        int Language) const
{
    BLogger::Log("Loading language");

    pContext->GetMenu().LoadLanguage(Language);

    for (auto it = pContext->GetMenu().labels.begin();
         it != pContext->GetMenu().labels.end(); ++it)
    {
        it->second.RecreateDisplayList();
    }

    pContext->GetConfig().Language = Language;
    pContext->GetConfig().WriteToFile();
}

