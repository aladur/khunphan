/*
    kpstatefinish.cpp

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
#include "kpstatefinish.h"
#include "kpmenu.h"
#include "kpuibase.h"


void KPstateFinish::Initialize(KPstateContext *pContext,
                               tKPMenuState previousStateId)
{
    KPstate::Initialize(pContext, previousStateId);

    UpdateDisplay(pContext);
}

void KPstateFinish::UpdateDisplay(KPstateContext *pContext) const
{
    KPstate::UpdateDisplay(pContext);

    auto &menu = pContext->GetMenu();

    menu.plates[PLATE_MENUBACKGROUND].SetPosition(4.5, 4.5, 11.5, 8.5);

    menu.plates[PLATE_LOGO].SetPosition(4, 9, 12, 11);
    menu.plates[PLATE_LOGO].SetFullyVisible();

    menu.labels[T_QUIT].SetPosition(8, 8, 1, A_CENTERED);
    menu.labels[T_QUIT].SetFullyVisible();
    menu.labels[T_DOQUIT].SetPosition(8, 6, 1, A_CENTERED);
    menu.labels[T_DOQUIT].SetSignal(S_FINISH);
    menu.labels[T_BACK].SetPosition(8, 1, 1, A_CENTERED);
    menu.labels[T_BACK].SetSignal(S_BACK);

    StartAnimation(pContext);
}

void  KPstateFinish::MouseClick(KPstateContext *pContext, tMouseButton button,
                                tMouseEvent event, int x, int y)
{
    auto Signal = KPstate::EvaluateMouseClick(pContext, button, event, x, y);

    switch (Signal)
    {
        case S_FINISH:
            pContext->GetUserInterface().RequestForClose();
            break;

        case S_BACK:
            pContext->ChangeState(pContext->GetPreviousState());
            break;
    }
}

void KPstateFinish::PlayAudioForInitialize(KPstateContext *pContext) const
{
    pContext->GetUserInterface().PlayAudio(KP_SND_QUITGAME);
}
