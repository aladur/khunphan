/*
    kpstatefinish.cpp

    Automatic solution finder for KhunPhan game
    Copyright (C) 2001-2024  W. Schwotzer

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
                               StateId previousStateId)
{
    KPstate::Initialize(pContext, previousStateId);

    UpdateDisplay(pContext);
}

void KPstateFinish::UpdateDisplay(KPstateContext *pContext) const
{
    KPstate::UpdateDisplay(pContext);

    auto &menu = pContext->GetMenu();

    menu.plates[KPPlate::MenuBackground].SetPosition(4.5, 4.5, 11.5, 8.5);

    menu.plates[KPPlate::Logo].SetPosition(4, 9, 12, 11);
    menu.plates[KPPlate::Logo].SetFullyVisible();

    menu.labels[Lbl::Quit].SetPosition(8, 8, 1, AlignItem::Centered);
    menu.labels[Lbl::Quit].SetFullyVisible();
    menu.labels[Lbl::DoQuit].SetPosition(8, 6, 1, AlignItem::Centered);
    menu.labels[Lbl::DoQuit].SetSignal(Signal::Quit);
    menu.labels[Lbl::Back].SetPosition(8, 1, 1, AlignItem::Centered);
    menu.labels[Lbl::Back].SetSignal(Signal::Back);

    StartAnimation(pContext);
}

void  KPstateFinish::MouseClick(KPstateContext *pContext, MouseButton button,
                                MouseButtonEvent event, int x, int y)
{
    auto signal = KPstate::EvaluateMouseClick(pContext, button, event, x, y);

    switch (signal)
    {
        case Signal::Quit:
            pContext->GetUserInterface().RequestForClose();
            break;

        case Signal::Back:
            pContext->ChangeState(pContext->GetPreviousState());
            return;

        default:
            break;
    }
}

void KPstateFinish::PlayAudioForInitialize(KPstateContext *pContext) const
{
    pContext->GetUserInterface().PlayAudio(KPSound::QuitGame);
}
