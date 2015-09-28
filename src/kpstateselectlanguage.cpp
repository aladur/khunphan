/*
    kpstateselectlanguage.cpp

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


#include "kpstateselectlanguage.h"
#include "kpmenu.h"
#include "kpconfig.h"


void KPstateSelectLanguage::Initialize(KPstateContext *pContext,
                                       const KPstate *pOldState)
{
    KPstate::Initialize(pContext, pOldState);

    // Do some initialization stuff here:

    UpdateDisplay(pContext);
}

void KPstateSelectLanguage::UpdateDisplay(KPstateContext *pContext)
{
    KPstate::UpdateDisplay(pContext);

    KPmenu &menu = pContext->GetMenu();

    menu.plates[PLATE_MENUBACKGROUND].SetPosition(2,2.5,14,8.5);

    menu.plates[PLATE_LOGO].SetPosition(5,9,11,11);
    menu.plates[PLATE_LOGO].SetFullyVisible();

    menu.labels[T_SELECTLANGUAGE].SetPosition(8,8,1,A_CENTERED);
    menu.labels[T_SELECTLANGUAGE].SetFullyVisible();

    float Pos_y=7;
    for (int i = T_LANGUAGE1; i <= T_LANGUAGE_MAX; i++)
    {
        if ( menu.labels.find(i) != menu.labels.end() )
        {
            menu.labels[i].SetPosition(8,Pos_y,0.71f,A_CENTERED);
            menu.labels[i].SetSignal(i);
            Pos_y-=.5;
        }
    }

    if (KPConfig::Instance().Language)
    {
        menu.labels[T_BACK].SetPosition(8,1,1,A_CENTERED);
        menu.labels[T_BACK].SetSignal(S_BACK);
    }

    StartAnimation();
}

void KPstateSelectLanguage::MouseClick (KPstateContext *pContext,
                                        tMouseButton button, tMouseEvent event,
                                        int x, int y)
{
    int Signal = KPstate::EvaluateMouseClick(pContext, button, event, x, y);

    switch (Signal)
    {
        case S_BACK:
            pContext->ChangeState(oldStateId);
            break;
    }

    // Language has been changed

    if (Signal >= T_LANGUAGE1 && Signal <= T_LANGUAGE_MAX)
    {
        SetLanguage(pContext, Signal);
        pContext->ChangeState(KPState_MainMenu);
    }
}

void KPstateSelectLanguage::KeyPressed (KPstateContext *pContext,
                                        unsigned char key, int x, int y)
{
    CHECK_DEFAULT_KEY_PRESSED(pContext, key, x, y);
}

tKPMenuState KPstateSelectLanguage::ESCKeyAction (KPstateContext *pContext)
{
    if (KPConfig::Instance().Language)
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

void KPstateSelectLanguage::SetLanguage (KPstateContext *pContext, int Language)
{
    pContext->GetMenu().LoadLanguage(Language);
    tIdToLabel::iterator it;

    for (it = pContext->GetMenu().labels.begin();
         it != pContext->GetMenu().labels.end(); ++it)
    {
        it->second.RecreateDisplayList();
    }

    KPConfig::Instance().Language = Language;
    KPConfig::Instance().WriteToFile();
}

