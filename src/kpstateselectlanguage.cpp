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
                                       StateId previousStateId)
{
    KPstate::Initialize(pContext, previousStateId);

    UpdateDisplay(pContext);
}

void KPstateSelectLanguage::UpdateDisplay(KPstateContext *pContext) const
{
    auto &menu = pContext->GetMenu();

    KPstate::UpdateDisplay(pContext);

    menu.plates[KPPlate::MenuBackground].SetPosition(2, 2.5, 14, 8.5);

    menu.plates[KPPlate::Logo].SetPosition(5, 9, 11, 11);
    menu.plates[KPPlate::Logo].SetFullyVisible();

    menu.labels[Lbl::SelectLanguage].SetPosition(8, 8, 1, AlignItem::Centered);
    menu.labels[Lbl::SelectLanguage].SetFullyVisible();

    auto Pos_y = 7.0f;

    for (auto labelId = Lbl::Language1; labelId != Lbl::LanguageMax;)
    {
        if (menu.labels.find(labelId) != menu.labels.end())
        {
            menu.labels[labelId].SetPosition(8, Pos_y, 0.71f,
                                             AlignItem::Centered);
            menu.labels[labelId].SetSignal(static_cast<std::size_t>(labelId));
            Pos_y -= .5;
        }

        labelId = GetLabelId(labelId, 1);
    }

    if (pContext->GetConfig().Language)
    {
        menu.labels[Lbl::Back].SetPosition(8, 1, 1, AlignItem::Centered);
        menu.labels[Lbl::Back].SetSignal(S_BACK);
    }

    StartAnimation(pContext);
}

void KPstateSelectLanguage::MouseClick(KPstateContext *pContext,
                                       MouseButton button,
                                       MouseButtonEvent event,
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
    if (Signal >= static_cast<int>(Lbl::Language1) &&
        Signal <= static_cast<int>(Lbl::LanguageMax))
    {
        SetLanguage(pContext, static_cast<Lbl>(Signal));
        pContext->ChangeState(StateId::MainMenu);
    }
}

StateId KPstateSelectLanguage::ESCKeyAction(KPstateContext *pContext) const
{
    if (pContext->GetConfig().Language)
    {
        return pContext->GetMenu().RestoreLastStateId();
    }
    else
    {
        // Default: Set Language to "English"
        SetLanguage(pContext, GetLabelId(Lbl::Language1, 1));
        return StateId::MainMenu;
    }
}

void KPstateSelectLanguage::SetLanguage(KPstateContext *pContext,
                                        Lbl Language) const
{
    BLogger::Log("Loading language");

    pContext->GetMenu().LoadLanguage(Language);

    for (auto &item : pContext->GetMenu().labels)
    {
        item.second.RecreateDisplayList();
    }

    pContext->GetConfig().Language = static_cast<int>(Language);
    pContext->GetConfig().WriteToFile();
}

