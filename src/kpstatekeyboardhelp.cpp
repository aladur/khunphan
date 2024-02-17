/*
    kpstatekeyboardhelp.cpp

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
#include "kpstatekeyboardhelp.h"
#include "kpmenu.h"
#include "kpuibase.h"

void KPstateKeyboardHelp::Initialize(KPstateContext *pContext,
                                     StateId previousStateId)
{
    KPstate::Initialize(pContext, previousStateId);

    UpdateDisplay(pContext);
}

void KPstateKeyboardHelp::UpdateDisplay(KPstateContext *pContext) const
{
    auto &menu = pContext->GetMenu();

    KPstate::UpdateDisplay(pContext);

    menu.plates[KPPlate::MenuBackground].SetPosition(2, 2.5, 14, 9.0);

    menu.plates[KPPlate::Logo].SetPosition(5, 9, 11, 11);
    menu.plates[KPPlate::Logo].SetFullyVisible();

    auto fullScreen = pContext->GetUserInterface().CanToggleFullScreen();

    auto y = 8.5f;
    menu.labels[Lbl::KeyboardHelp].SetPosition(8, y, 1, AlignItem::Centered);
    y -= 1.0f;
    menu.labels[Lbl::KeyboardHelp].SetFullyVisible();

    menu.labels[Lbl::HelpGeneral].SetPosition(8, y, 1, AlignItem::Centered);
    y -= 1.0f;
    menu.labels[Lbl::HelpKeyESC].SetPosition(3, y, 0.6f);
    y -= 0.6f;
    menu.labels[Lbl::HelpKeyOpenGL].SetPosition(3, y, 0.6f);
    y -= (fullScreen ? 0.6f : 1.0f);

    if (fullScreen)
    {
        menu.labels[Lbl::HelpKeyFullscreen].SetPosition(3, y, 0.6f);
        y -= 1.0f;
    }

    menu.labels[Lbl::HelpInGame].SetPosition(8, y, 1, AlignItem::Centered);
    y -= 1.0f;
    menu.labels[Lbl::HelpKeyPause].SetPosition(3, y, 0.6f);
    y -= 0.6f;
    menu.labels[Lbl::HelpKeyCamera].SetPosition(3, y, 0.6f);

    y = 6.5f;
    menu.labels[Lbl::HelpESC].SetPosition(6, y, 0.6f);
    y -= 0.6f;
    menu.labels[Lbl::HelpOpenGL].SetPosition(6, y, 0.6f);
    y -= (fullScreen ? 0.6f : 2.0f);

    if (fullScreen)
    {
        menu.labels[Lbl::HelpFullscreen].SetPosition(6, y, 0.6f);
        y -= 2.0f;
    }

    menu.labels[Lbl::HelpPause].SetPosition(6, y, 0.6f);
    y -= 0.6f;
    menu.labels[Lbl::HelpCamera].SetPosition(6, y, 0.6f);

    menu.labels[Lbl::Back].SetPosition(8, 1, 1, AlignItem::Centered);
    menu.labels[Lbl::Back].SetSignal(Signal::Back);

    StartAnimation(pContext);
}

void  KPstateKeyboardHelp::MouseClick(KPstateContext *pContext,
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
}
