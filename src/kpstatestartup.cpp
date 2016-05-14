/*
    kpstatestartup.cpp

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
#include "kpstatestartup.h"
#include "kpmenu.h"
#include "camera.h"
#include "light.h"
#include "kpboardGL.h"
#include "kpconfig.h"
#include "kpuibase.h"
#include "language.h"

KPstateStartUp::KPstateStartUp()
{
}

void KPstateStartUp::Initialize(KPstateContext *pContext,
                                StateId previousStateId)
{
    KPstate::Initialize(pContext, previousStateId);

    auto it = pContext->GetMenu().labels.find(T_COPYRIGHT2);

    if (it == pContext->GetMenu().labels.end())
    {
        pContext->GetMenu().labels[T_COPYRIGHT2] =
            Label("(C) 2002-2016 Wolfgang Schwotzer");
    }

    it = pContext->GetMenu().labels.find(T_COPYRIGHT3);

    if (it == pContext->GetMenu().labels.end())
    {
        pContext->GetMenu().labels[T_COPYRIGHT3] = Label();
    }

    it = pContext->GetMenu().labels.find(T_COPYRIGHT5);

    if (it == pContext->GetMenu().labels.end())
    {
        pContext->GetMenu().labels[T_COPYRIGHT5] =
            Label("(C) 2001-2006 Nicklas Nygren");
    }

    it = pContext->GetMenu().labels.find(T_COPYRIGHT6);

    if (it == pContext->GetMenu().labels.end())
    {
        pContext->GetMenu().labels[T_COPYRIGHT6] =
            Label("http://nifflas.ni2.se/");
    }

    UpdateDisplay(pContext);

    pContext->GetCamera().SetRoundtrip(true);
}

void KPstateStartUp::UpdateDisplay(KPstateContext *pContext) const
{
    auto &menu = pContext->GetMenu();

    KPstate::UpdateDisplay(pContext);

    // every thing which should be permanently be visible within this
    // state should be listed here:
    menu.plates[KPPlate::Logo].SetPosition(5, 9, 11, 11);

    auto y = 3.0f;
    menu.labels[T_COPYRIGHT1].SetPosition(8, y, 0.4f, AlignItem::Centered);
    y -= 0.4f;
    menu.labels[T_COPYRIGHT2].SetPosition(8, y, 0.4f, AlignItem::Centered);
    y -= 0.4f;
    menu.labels[T_COPYRIGHT3].SetPosition(8, y, 0.4f, AlignItem::Centered);
    y -= 0.8f;
    menu.labels[T_COPYRIGHT4].SetPosition(8, y, 0.4f, AlignItem::Centered);
    y -= 0.4f;
    menu.labels[T_COPYRIGHT5].SetPosition(8, y, 0.4f, AlignItem::Centered);
    y -= 0.4f;
    menu.labels[T_COPYRIGHT6].SetPosition(8, y, 0.4f, AlignItem::Centered);

    menu.labels[T_NOWARRANTY].SetPosition(8, 0, 0.5, AlignItem::Centered);
    menu.labels[T_VERSION].SetPosition(0.1f, 0, 0.5);

    StartAnimation(pContext);
}

StateId KPstateStartUp::ESCKeyAction(KPstateContext *pContext) const
{
    pContext->GetUserInterface().RequestForClose();

    return StateId::Invalid;
}

void KPstateStartUp::MouseClick(KPstateContext *pContext,
                                MouseButton, MouseButtonEvent event,
                                int, int)
{
    if (event == MouseButtonEvent::Release)
    {
        if (pContext->GetConfig().Language)
        {
            pContext->ChangeState(StateId::MainMenu);
        }
        else
        {
            pContext->ChangeState(StateId::SelectLanguage);
        }
    }
}

void KPstateStartUp::Draw(KPstateContext *pContext) const
{
    pContext->GetCamera().Draw();

    pContext->GetBoardView().Draw();

    pContext->GetMenu().Draw();
}

