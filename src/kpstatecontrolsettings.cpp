/*
    kpstatecontrolsettings.cpp

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
#include "kpstatecontrolsettings.h"
#include "kpmenu.h"
#include "kpconfig.h"
#include "kpuibase.h"


void KPstateControlSettings::Initialize(KPstateContext *pContext,
                                        StateId previousStateId)
{
    KPstate::Initialize(pContext, previousStateId);

    // Do some initialization stuff here:

    E_MouseSpeed   = pContext->GetConfig().MouseSpeed;
    E_SolutionHint = pContext->GetConfig().SolutionHint;

    UpdateDisplay(pContext);
}

void KPstateControlSettings::UpdateDisplay(KPstateContext *pContext) const
{
    auto &menu = pContext->GetMenu();
    const auto dy = 0.6f;

    KPstate::UpdateDisplay(pContext);

    menu.plates[KPPlate::MenuBackground].SetPosition(2, 3.5, 14, 8.5);

    menu.plates[KPPlate::Logo].SetPosition(4, 9, 12, 11);
    menu.plates[KPPlate::Logo].SetFullyVisible();

    menu.labels[Lbl::Settings].SetPosition(4, 8, 1);
    menu.labels[Lbl::Settings].SetFullyVisible();
    menu.labels[Lbl::Control].SetPosition(12, 8, 1, AlignItem::Right);
    menu.labels[Lbl::Control].SetFullyVisible();

    auto y = 6.0f;
    menu.labels[Lbl::MouseSpeed].SetPosition(8, y, 0.71f, AlignItem::Right);
    menu.labels[Lbl::MouseSpeed].SetSignal(S_TOGGLE_MOUSE_SPEED);

    if (E_MouseSpeed <= .293)
    {
        menu.labels[Lbl::MouseSpeedSlow].SetPosition(8.2f, y, 0.71f);
        menu.labels[Lbl::MouseSpeedSlow].SetSignal(S_TOGGLE_MOUSE_SPEED);
    }
    else if (E_MouseSpeed <= .41)
    {
        menu.labels[Lbl::MouseSpeedMedium].SetPosition(8.2f, y, 0.71f);
        menu.labels[Lbl::MouseSpeedMedium].SetSignal(S_TOGGLE_MOUSE_SPEED);
    }
    else if (E_MouseSpeed <= .575)
    {
        menu.labels[Lbl::MouseSpeedFast].SetPosition(8.2f, y, 0.71f);
        menu.labels[Lbl::MouseSpeedFast].SetSignal(S_TOGGLE_MOUSE_SPEED);
    }
    else
    {
        menu.labels[Lbl::MouseSpeedVeryFast].SetPosition(8.2f, y, 0.71f);
        menu.labels[Lbl::MouseSpeedVeryFast].SetSignal(S_TOGGLE_MOUSE_SPEED);
    }

    y -= dy;
    menu.labels[Lbl::SolutionHint].SetPosition(8, y, 0.71f, AlignItem::Right);
    menu.labels[Lbl::SolutionHint].SetSignal(S_TOGGLE_SOLUTION_HINT);

    if (E_SolutionHint)
    {
        menu.labels[Lbl::SolutionHintOn].SetPosition(8.2f, y, 0.71f);
        menu.labels[Lbl::SolutionHintOn].SetSignal(S_TOGGLE_SOLUTION_HINT);
    }
    else
    {
        menu.labels[Lbl::SolutionHintOff].SetPosition(8.2f, y, 0.71f);
        menu.labels[Lbl::SolutionHintOff].SetSignal(S_TOGGLE_SOLUTION_HINT);
    }

    menu.labels[Lbl::Back].SetPosition(8, 1, 1, AlignItem::Centered);
    menu.labels[Lbl::Back].SetSignal(S_BACK);

    StartAnimation(pContext);
}

void KPstateControlSettings::MouseClick(KPstateContext *pContext,
                                        MouseButton button,
                                        MouseButtonEvent event,
                                        int x, int y)
{
    auto Signal = KPstate::EvaluateMouseClick(pContext, button, event, x, y);

    switch (Signal)
    {
        case S_TOGGLE_SOLUTION_HINT:
            ToggleSolutionHint(pContext);
            UpdateDisplay(pContext);
            break;

        case S_TOGGLE_MOUSE_SPEED:
            ToggleMouseSpeed(pContext);
            UpdateDisplay(pContext);
            break;

        case S_BACK:
            SaveChanges(pContext);
            pContext->ChangeState(pContext->GetPreviousState());
            break;
    }
}

/////////////////////////////////////////////////////////////////////
// Toggle settings
/////////////////////////////////////////////////////////////////////

void  KPstateControlSettings::ToggleMouseSpeed(KPstateContext *pContext)
{
    pContext->GetUserInterface().PlayAudio(KPSound::ChangeSetting);

    if (E_MouseSpeed <= 0.21f)
    {
        E_MouseSpeed = 0.8f;
    }
    else if (E_MouseSpeed <= 0.29f)
    {
        E_MouseSpeed = 0.8f;
    }
    else if (E_MouseSpeed <= 0.41f)
    {
        E_MouseSpeed = 0.28f;
    }
    else if (E_MouseSpeed <= 0.575f)
    {
        E_MouseSpeed = 0.4f;
    }
    else
    {
        E_MouseSpeed = 0.565f;
    }
}

void  KPstateControlSettings::ToggleSolutionHint(KPstateContext *pContext)
{
    pContext->GetUserInterface().PlayAudio(KPSound::ChangeSetting);
    E_SolutionHint = E_SolutionHint ? 0 : 1;
}

StateId KPstateControlSettings::ESCKeyAction(
    KPstateContext *pContext) const
{
    SaveChanges(pContext);
    return pContext->GetPreviousState();
}

void KPstateControlSettings::SaveChanges(KPstateContext *pContext) const
{
    pContext->GetConfig().MouseSpeed   = E_MouseSpeed;
    pContext->GetConfig().SolutionHint = E_SolutionHint;

    pContext->GetConfig().WriteToFile();
}
