/*
    kpstatekeyboardhelp.cpp

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
#include "kpstatekeyboardhelp.h"
#include "kpmenu.h"
#include "kpuibase.h"

void KPstateKeyboardHelp::Initialize(KPstateContext *pContext,
                                     tKPMenuState previousStateId)
{
    KPstate::Initialize(pContext, previousStateId);

    UpdateDisplay(pContext);
}

void KPstateKeyboardHelp::UpdateDisplay(KPstateContext *pContext) const
{
    auto &menu = pContext->GetMenu();

    KPstate::UpdateDisplay(pContext);

    menu.plates[PLATE_MENUBACKGROUND].SetPosition(2, 2.5, 14, 9.0);

    menu.plates[PLATE_LOGO].SetPosition(5, 9, 11, 11);
    menu.plates[PLATE_LOGO].SetFullyVisible();

    auto fullScreen = pContext->GetUserInterface().CanToggleFullScreen();

    auto y = 8.5f;
    menu.labels[T_H_KEYBOARDHELP].SetPosition(8, y, 1, A_CENTERED);
    y -= 1.0f;
    menu.labels[T_H_KEYBOARDHELP].SetFullyVisible();

    menu.labels[T_H_GENERAL].SetPosition(8, y, 1, A_CENTERED);
    y -= 1.0f;
    menu.labels[T_H_ESC_KEY].SetPosition(3, y, 0.6f);
    y -= 0.6f;
    menu.labels[T_H_OPENGL_KEY].SetPosition(3, y, 0.6f);
    y -= (fullScreen ? 0.6f : 1.0f);

    if (fullScreen)
    {
        menu.labels[T_H_FULLSCREEN_KEY].SetPosition(3, y, 0.6f);
        y -= 1.0f;
    }

    menu.labels[T_H_IN_GAME].SetPosition(8, y, 1, A_CENTERED);
    y -= 1.0f;
    menu.labels[T_H_PAUSE_KEY].SetPosition(3, y, 0.6f);
    y -= 0.6f;
    menu.labels[T_H_CAMERA_KEY].SetPosition(3, y, 0.6f);
    y -= 0.6f;

    y = 6.5f;
    menu.labels[T_H_ESC].SetPosition(6, y, 0.6f);
    y -= 0.6f;
    menu.labels[T_H_OPENGL].SetPosition(6, y, 0.6f);
    y -= (fullScreen ? 0.6f : 2.0f);

    if (fullScreen)
    {
        menu.labels[T_H_FULLSCREEN].SetPosition(6, y, 0.6f);
        y -= 2.0f;
    }

    menu.labels[T_H_PAUSE].SetPosition(6, y, 0.6f);
    y -= 0.6f;
    menu.labels[T_H_CAMERA].SetPosition(6, y, 0.6f);

    menu.labels[T_BACK].SetPosition(8, 1, 1, A_CENTERED);
    menu.labels[T_BACK].SetSignal(S_BACK);

    StartAnimation(pContext);
}

void  KPstateKeyboardHelp::MouseClick(KPstateContext *pContext,
                                      tMouseButton button, tMouseEvent event,
                                      int x, int y)
{
    auto Signal = KPstate::EvaluateMouseClick(pContext, button, event, x, y);

    switch (Signal)
    {
        case S_BACK:
            pContext->ChangeState(pContext->GetPreviousState());
            break;
    }
}
