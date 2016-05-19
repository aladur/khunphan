/*
    kpstatelighttest.cpp

    Automatic solution finder for KhunPhan game
    Copyright (C) 2006-2016  W. Schwotzer

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
#include "kpstatelighttest.h"
#include "kpmenu.h"
#include "light.h"
#include "camera.h"
#include "kpuibase.h"
#include "kpconfig.h"
#include "blogger.h"


KPstateLightTest::KPstateLightTest() : mouse_x(0), mouse_y(0)
{
}

void KPstateLightTest::Initialize(KPstateContext *pContext,
                                  StateId previousStateId)
{
    KPstate::Initialize(pContext, previousStateId);

    pContext->GetConfig().CameraPosition = 1;
    pContext->GetCamera().SetPosition(pContext->GetConfig().CameraPosition);

    UpdateDisplay(pContext);
}

void KPstateLightTest::UpdateDisplay(KPstateContext *pContext) const
{
    auto &menu = pContext->GetMenu();

    KPstate::UpdateDisplay(pContext);

    menu.labels[Lbl::Ok].SetPosition(8, 1, 1, AlignItem::Centered);
    menu.labels[Lbl::Ok].SetSignal(Signal::Back);

    StartAnimation(pContext);
}

void  KPstateLightTest::KeyPressed(KPstateContext *pContext, unsigned char key,
                                   int x, int y) const
{
    switch (key)
    {
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
            pContext->GetConfig().CameraPosition = key - '1' + 1;
            pContext->GetCamera().SetPosition(
                pContext->GetConfig().CameraPosition);
            break;
    }

    KPstate::KeyPressed(pContext, key, x, y);
}

StateId KPstateLightTest::ESCKeyAction(KPstateContext *pContext) const
{
    pContext->GetUserInterface().RequestForClose();

    return StateId::Invalid;
}

void  KPstateLightTest::MouseClick(KPstateContext *pContext,
                                   MouseButton button, MouseButtonEvent event,
                                   int x, int y)
{
    auto signal = KPstate::EvaluateMouseClick(pContext, button, event, x, y);

    switch (signal)
    {
        case Signal::Back:
            pContext->GetUserInterface().RequestForClose();
            break;

        default:
            HandleMouseClick(pContext, button, event, x, y);
            break;
    }
}

void KPstateLightTest::HandleMouseClick(KPstateContext *pContext,
                                        MouseButton button,
                                        MouseButtonEvent event,
                                        int x, int y)
{
    float lx, ly, lz;

    if (button == MouseButton::Left && event == MouseButtonEvent::Press)
    {
        mouse_x = x; // save current mouse position
        mouse_y = y;
    }
    else if (button == MouseButton::Left && event == MouseButtonEvent::Release)
    {
        // calculate mouse direction
        auto diff_x = x - mouse_x;
        auto diff_z = -(y - mouse_y);
        pContext->GetLight().GetPosition(lx, ly, lz);
        lx += diff_x;
        lz += diff_z;
        pContext->GetLight().SetPosition(lx, ly, lz);
        BLogger::Log(std::fixed, "Light position x=", lx, " y=", ly, " z=", lz);
    }
    else if (button == MouseButton::Right && event == MouseButtonEvent::Press)
    {
        mouse_x = x; // save current mouse position
        mouse_y = y;
    }
    else if (button == MouseButton::Right && event == MouseButtonEvent::Release)
    {
        // calculate mouse direction
        auto diff_y = -(y - mouse_y);
        pContext->GetLight().GetPosition(lx, ly, lz);
        ly += diff_y;
        pContext->GetLight().SetPosition(lx, ly, lz);
        BLogger::Log(std::fixed, "Light position x=", lx, " y=", ly, " z=", lz);
    }
}

