/*
    kpstatesettings.cpp

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
#include "kpstatesettings.h"
#include "kpmenu.h"
#include "kpconfig.h"


void KPstateSettings::Initialize(KPstateContext *pContext,
                                 StateId previousStateId)
{
    KPstate::Initialize(pContext, previousStateId);

    UpdateDisplay(pContext);
}

void KPstateSettings::UpdateDisplay(KPstateContext *pContext) const
{
    auto &menu = pContext->GetMenu();

    KPstate::UpdateDisplay(pContext);

    menu.plates[KPPlate::MenuBackground].SetPosition(4, 2.5, 12, 8.5);
    menu.plates[KPPlate::Logo].SetPosition(4, 9, 12, 11);
    menu.plates[KPPlate::Logo].SetFullyVisible();

    menu.labels[Lbl::Settings].SetPosition(8, 8, 1, AlignItem::Centered);
    menu.labels[Lbl::Settings].SetFullyVisible();

    auto y = 6.5f;
    menu.labels[Lbl::Control].SetPosition(8, y, 1, AlignItem::Centered);
    y -= 1.0f;
    menu.labels[Lbl::Control].SetSignal(Signal::Control);

    menu.labels[Lbl::Graphics].SetPosition(8, y, 1, AlignItem::Centered);
    y -= 1.0f;
    menu.labels[Lbl::Graphics].SetSignal(Signal::Graphics);

    menu.labels[Lbl::Audio].SetPosition(8, y, 1, AlignItem::Centered);
    y -= 1.0f;
    menu.labels[Lbl::Audio].SetSignal(Signal::Audio);

    menu.labels[Lbl::Language].SetPosition(8, y, 1, AlignItem::Centered);
    menu.labels[Lbl::Language].SetSignal(Signal::Language);

    menu.labels[Lbl::Back].SetPosition(8, 1, 1, AlignItem::Centered);
    menu.labels[Lbl::Back].SetSignal(Signal::Back);

    StartAnimation(pContext);
}

void  KPstateSettings::MouseClick(KPstateContext *pContext,
                                  MouseButton button, MouseButtonEvent event,
                                  int x, int y)
{
    auto signal = KPstate::EvaluateMouseClick(pContext, button, event, x, y);

    switch (signal)
    {
        case Signal::Control:
            pContext->ChangeState(StateId::ControlSettings);
            return;

        case Signal::Graphics:
            pContext->ChangeState(StateId::GraphicSettings);
            return;

        case Signal::Audio:
            pContext->ChangeState(StateId::AudioSettings);
            return;

        case Signal::Language:
            pContext->ChangeState(StateId::SelectLanguage);
            return;

        case Signal::Back:
            pContext->ChangeState(ESCKeyAction(pContext));
            return;

        default:
            break;
    }
}

StateId KPstateSettings::ESCKeyAction(KPstateContext *pContext) const
{
    return pContext->GetMenu().RestoreLastStateId();
}
