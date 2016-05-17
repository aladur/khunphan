/*
    kpstatesettings.cpp

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
    y -= 1.0;
    menu.labels[Lbl::Control].SetSignal(S_CONTROL_SETTINGS);

    menu.labels[Lbl::Graphics].SetPosition(8, y, 1, AlignItem::Centered);
    y -= 1.0;
    menu.labels[Lbl::Graphics].SetSignal(S_GRAPHIC_SETTINGS);

    menu.labels[Lbl::Audio].SetPosition(8, y, 1, AlignItem::Centered);
    y -= 1.0;
    menu.labels[Lbl::Audio].SetSignal(S_AUDIO_SETTINGS);

    menu.labels[Lbl::Language].SetPosition(8, y, 1, AlignItem::Centered);
    menu.labels[Lbl::Language].SetSignal(S_SELECT_LANGUAGE);

    menu.labels[Lbl::Back].SetPosition(8, 1, 1, AlignItem::Centered);
    menu.labels[Lbl::Back].SetSignal(S_BACK);

    StartAnimation(pContext);
}

void  KPstateSettings::MouseClick(KPstateContext *pContext,
                                  MouseButton button, MouseButtonEvent event,
                                  int x, int y)
{
    auto Signal = KPstate::EvaluateMouseClick(pContext, button, event, x, y);

    switch (Signal)
    {
        case S_CONTROL_SETTINGS:
            pContext->ChangeState(StateId::ControlSettings);
            break;

        case S_GRAPHIC_SETTINGS:
            pContext->ChangeState(StateId::GraphicSettings);
            break;

        case S_AUDIO_SETTINGS:
            pContext->ChangeState(StateId::AudioSettings);
            break;

        case S_SELECT_LANGUAGE:
            pContext->ChangeState(StateId::SelectLanguage);
            break;

        case S_BACK:
            pContext->ChangeState(ESCKeyAction(pContext));
            break;
    }
}

StateId KPstateSettings::ESCKeyAction(KPstateContext *pContext) const
{
    return pContext->GetMenu().RestoreLastStateId();
}
