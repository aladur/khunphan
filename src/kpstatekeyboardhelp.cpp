/*
    kpstatekeyboardhelp.h

    Automatic solution finder for KhunPhan game
    Copyright (C) 2001-2015  W. Schwotzer

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


#include "kpstatekeyboardhelp.h"
#include "kpmenu.h"
#include "kpuibase.h"

void KPstateKeyboardHelp::Initialize(KPstateContext *pContext,
                                     const KPstate *pOldState)
{
    KPstate::Initialize(pContext, pOldState);

    // Do some initialization stuff here:

    UpdateDisplay(pContext);
}

void KPstateKeyboardHelp::UpdateDisplay(KPstateContext *pContext)
{
    KPstate::UpdateDisplay(pContext);

    KPmenu &menu = pContext->GetMenu();

    menu.plates[SHLD_MENUBACKGROUND].Positioniere(2,2.5,14,9.0);

    menu.plates[SHLD_LOGO].Positioniere(5,9,11,11);
    menu.plates[SHLD_LOGO].VollSichtbar();

    bool fullScreen = pContext->GetUserInterface().CanToggleFullScreen();

    float y = 8.5;
    menu.labels[T_H_KEYBOARDHELP].Positioniere(8,y,1,A_CENTERED);
    y -= 1.0f;
    menu.labels[T_H_KEYBOARDHELP].VollSichtbar();

    menu.labels[T_H_GENERAL].Positioniere(8,y,1,A_CENTERED);
    y -= 1.0f;
    menu.labels[T_H_ESC_KEY].Positioniere(3,y,0.6f);
    y -= 0.6f;
    menu.labels[T_H_OPENGL_KEY].Positioniere(3,y,0.6f);
    y -= (fullScreen ? 0.6f : 1.0f);
    if (fullScreen)
    {
        menu.labels[T_H_FULLSCREEN_KEY].Positioniere(3,y,0.6f);
        y -= 1.0f;
    }
    menu.labels[T_H_IN_GAME].Positioniere(8,y,1,A_CENTERED);
    y -= 1.0f;
    menu.labels[T_H_PAUSE_KEY].Positioniere(3,y,0.6f);
    y -= 0.6f;
    menu.labels[T_H_CAMERA_KEY].Positioniere(3,y,0.6f);
    y -= 0.6f;

    y = 6.5f;
    menu.labels[T_H_ESC].Positioniere(6,y,0.6f);
    y -= 0.6f;
    menu.labels[T_H_OPENGL].Positioniere(6,y,0.6f);
    y -= (fullScreen ? 0.6f : 2.0f);
    if (fullScreen)
    {
        menu.labels[T_H_FULLSCREEN].Positioniere(6,y,0.6f);
        y -= 2.0f;
    }
    menu.labels[T_H_PAUSE].Positioniere(6,y,0.6f);
    y -= 0.6f;
    menu.labels[T_H_CAMERA].Positioniere(6,y,0.6f);

    menu.labels[T_BACK].Positioniere(8,1,1,A_CENTERED);
    menu.labels[T_BACK].SetzeSignal(S_BACK);

    StartAnimation();
}

void KPstateKeyboardHelp::KeyPressed (KPstateContext *pContext,
                                      unsigned char key, int x, int y)
{
    CHECK_DEFAULT_KEY_PRESSED(pContext, key, x, y);
}

void  KPstateKeyboardHelp::MouseClick (KPstateContext *pContext,
                                       tMouseButton button, tMouseEvent event,
                                       int x, int y)
{
    int Signal = KPstate::EvaluateMouseClick(pContext, button, event, x, y);

    switch (Signal)
    {
        case S_BACK:
            pContext->ChangeState(oldStateId);
            break;
    }
}
