/*
    kpstategraphichint.cpp

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
#include "kpstategraphichint.h"
#include "kpmenu.h"


void KPstateGraphicHint::Initialize(KPstateContext *pContext,
                                    StateId previousStateId)
{
    KPstate::Initialize(pContext, previousStateId);

    UpdateDisplay(pContext);
}

void KPstateGraphicHint::UpdateDisplay(KPstateContext *pContext) const
{
    auto &menu = pContext->GetMenu();

    KPstate::UpdateDisplay(pContext);

    menu.plates[KPPlate::MenuBackground].SetPosition(2, 3.5, 15, 8.5);

    menu.plates[KPPlate::Logo].SetPosition(4, 9, 12, 11);
    menu.plates[KPPlate::Logo].SetFullyVisible();

    menu.labels[Lbl::Hint].SetPosition(8, 8, 1, AlignItem::Centered);
    menu.labels[Lbl::Hint].SetFullyVisible();

    menu.labels[Lbl::Hint1].SetPosition(8, 6.0, 0.7f, AlignItem::Centered);
    menu.labels[Lbl::Hint2].SetPosition(8, 5.5, 0.7f, AlignItem::Centered);

    if (menu.labels.find(Lbl::Hint3) != menu.labels.end())
    {
        menu.labels[Lbl::Hint3].SetPosition(8, 5.0, 0.7f, AlignItem::Centered);
    }

    menu.labels[Lbl::Ok].SetPosition(8, 1, 1, AlignItem::Centered);
    menu.labels[Lbl::Ok].SetSignal(Signal::Back);

    StartAnimation(pContext);
}

void  KPstateGraphicHint::MouseClick(KPstateContext *pContext,
                                     MouseButton button,
                                     MouseButtonEvent event,
                                     int x, int y)
{
    auto signal = KPstate::EvaluateMouseClick(pContext, button, event, x, y);

    switch (signal)
    {
        case Signal::Back:
            pContext->ChangeState(StateId::Settings);
            return;

        default:
            break;
    }
}

StateId KPstateGraphicHint::ESCKeyAction(KPstateContext *) const
{
    return StateId::Settings;
}
