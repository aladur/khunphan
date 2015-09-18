/*
    kpstatecontrolsettings.h

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

#include "kpstatecontrolsettings.h"
#include "kpmenu.h"
#include "kpconfig.h"
#include "kpuibase.h"


void KPstateControlSettings::Initialize(KPstateContext *pContext,
                                        const KPstate *pOldState)
{
    KPstate::Initialize(pContext, pOldState);

    // Do some initialization stuff here:

    E_MouseSpeed   = KPConfig::Instance().MouseSpeed;
    E_SolutionHint = KPConfig::Instance().SolutionHint;

    UpdateDisplay(pContext);
}

void KPstateControlSettings::UpdateDisplay(KPstateContext *pContext)
{
    KPstate::UpdateDisplay(pContext);

    KPmenu &menu = pContext->GetMenu();

    float y;
    float dy = 0.6f;

    menu.SchildArray[SHLD_MENUBACKGROUND].Positioniere(2,3.5,14,8.5);

    menu.SchildArray[SHLD_LOGO].Positioniere(4,9,12,11);
    menu.SchildArray[SHLD_LOGO].VollSichtbar();

    menu.TextfeldArray[T_EINSTELLUNGEN].Positioniere(4,8,1);
    menu.TextfeldArray[T_EINSTELLUNGEN].VollSichtbar();
    menu.TextfeldArray[T_STEUERUNG].Positioniere(12,8,1,A_RECHTS);
    menu.TextfeldArray[T_STEUERUNG].VollSichtbar();

    y = 6.0;
    menu.TextfeldArray[T_MAUSGESCHWINDIGKEIT].Positioniere(8, y, 0.71f, A_RECHTS);
    menu.TextfeldArray[T_MAUSGESCHWINDIGKEIT].SetzeSignal(
        S_TOGGLE_MOUSE_SPEED);

    if (E_MouseSpeed<=.293)
    {
        menu.TextfeldArray[T_MG_LANGSAM].Positioniere(8.2f, y, 0.71f);
        menu.TextfeldArray[T_MG_LANGSAM].SetzeSignal(S_TOGGLE_MOUSE_SPEED);
    }
    else if (E_MouseSpeed<=.41)
    {
        menu.TextfeldArray[T_MG_NORMAL].Positioniere(8.2f, y, 0.71f);
        menu.TextfeldArray[T_MG_NORMAL].SetzeSignal(S_TOGGLE_MOUSE_SPEED);
    }
    else if (E_MouseSpeed<=.575)
    {
        menu.TextfeldArray[T_MG_SCHNELL].Positioniere(8.2f, y, 0.71f);
        menu.TextfeldArray[T_MG_SCHNELL].SetzeSignal(S_TOGGLE_MOUSE_SPEED);
    }
    else
    {
        menu.TextfeldArray[T_MG_SEHRSCHNELL].Positioniere(8.2f, y, 0.71f);
        menu.TextfeldArray[T_MG_SEHRSCHNELL].SetzeSignal(S_TOGGLE_MOUSE_SPEED);
    }

    y -= dy;
    menu.TextfeldArray[T_SOLUTIONHINT].Positioniere(8, y, 0.71f, A_RECHTS);
    menu.TextfeldArray[T_SOLUTIONHINT].SetzeSignal(S_TOGGLE_SOLUTION_HINT);

    if (E_SolutionHint)
    {
        menu.TextfeldArray[T_SH_ON].Positioniere(8.2f, y, 0.71f);
        menu.TextfeldArray[T_SH_ON].SetzeSignal(S_TOGGLE_SOLUTION_HINT);
    }
    else
    {
        menu.TextfeldArray[T_SH_OFF].Positioniere(8.2f, y, 0.71f);
        menu.TextfeldArray[T_SH_OFF].SetzeSignal(S_TOGGLE_SOLUTION_HINT);
    }

    menu.TextfeldArray[T_ZURUECK].Positioniere(8,1,1,A_MITTE);
    menu.TextfeldArray[T_ZURUECK].SetzeSignal(S_BACK);

    StartAnimation();
}

void KPstateControlSettings::KeyPressed (KPstateContext *pContext,
        unsigned char key, int x, int y)
{
    CHECK_DEFAULT_KEY_PRESSED(pContext, key, x, y);
}

void KPstateControlSettings::MouseClick (KPstateContext *pContext,
        tMouseButton button, tMouseEvent event,
        int x, int y)
{
    int Signal = KPstate::EvaluateMouseClick(pContext, button, event, x, y);

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
            pContext->ChangeState(oldStateId);
            break;
    }
}

/////////////////////////////////////////////////////////////////////
// Toggle settings
/////////////////////////////////////////////////////////////////////

void  KPstateControlSettings::ToggleMouseSpeed(KPstateContext *pContext)
{
    pContext->GetUserInterface().PlayAudio(KP_SND_CHANGESETTING);
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
    pContext->GetUserInterface().PlayAudio(KP_SND_CHANGESETTING);
    E_SolutionHint = E_SolutionHint ? 0 : 1;
}

tKPMenuState KPstateControlSettings::ESCKeyAction (KPstateContext *pContext)
{
    SaveChanges(pContext);
    return oldStateId;
}

void KPstateControlSettings::SaveChanges(KPstateContext *)
{
    KPConfig::Instance().MouseSpeed   = E_MouseSpeed;
    KPConfig::Instance().SolutionHint = E_SolutionHint;

    KPConfig::Instance().WriteToFile();
}
