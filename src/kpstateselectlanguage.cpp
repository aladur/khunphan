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
#include "kpsignals.h"
#include "kplocale.h"


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

    const auto idsToStrings = GetLanguageList(pContext);

    for (auto &item : idsToStrings)
    {
        auto id = static_cast<std::size_t>(item.first);
        auto signal = static_cast<Signal>(id);

        menu.labels[item.first].SetPosition(8, Pos_y, 0.71f,
                                            AlignItem::Centered);
        menu.labels[item.first].SetSignal(signal);
        Pos_y -= 0.5;
    }

    // If the config not yet contains a language (first start)
    // the user is forced to set the language.
    // Otherwise the language selection can be quit:
    if (pContext->GetConfig().Language)
    {
        menu.labels[Lbl::Back].SetPosition(8, 1, 1, AlignItem::Centered);
        menu.labels[Lbl::Back].SetSignal(Signal::Back);
    }

    StartAnimation(pContext);
}

void KPstateSelectLanguage::MouseClick(KPstateContext *pContext,
                                       MouseButton button,
                                       MouseButtonEvent event,
                                       int x, int y)
{
    auto signal = KPstate::EvaluateMouseClick(pContext, button, event, x, y);

    switch (signal)
    {
        case Signal::Back:
            pContext->ChangeState(pContext->GetPreviousState());
            return;

        default:
            break;
    }

    const auto idsToStrings = GetLanguageList(pContext);

    for (auto &item : idsToStrings)
    {
        auto id = static_cast<std::size_t>(item.first);
        auto signalSelectLanguage = static_cast<Signal>(id);

        if (signal == signalSelectLanguage)
        {
            SetLanguage(pContext, signal);
            pContext->ChangeState(StateId::MainMenu);
            return;
        }
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
        SetLanguage(pContext, Signal::SelectEnglish);
        return StateId::MainMenu;
    }
}

void KPstateSelectLanguage::SetLanguage(KPstateContext *pContext,
                                        Signal Language) const
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

const tIdToString KPstateSelectLanguage::GetLanguageList(
    KPstateContext *pContext) const
{
    std::stringstream stream;
    stream << pContext->GetConfig().Get(KPDirectory::Locale)
           << 999
           << KPlocale::FileExtension;
    auto file = stream.str();

    auto idsToStrings = KPlocale::ReadFromFile(file);

    if (idsToStrings.empty())
    {
        throw std::runtime_error("Error loading language list.");
    }

    return idsToStrings;
}

