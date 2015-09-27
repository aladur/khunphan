/*
    kpstategraphichint.h

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

#include "kpstategraphichint.h"
#include "kpmenu.h"


void KPstateGraphicHint::Initialize(KPstateContext *pContext,
                                    const KPstate *pOldState)
{
    KPstate::Initialize(pContext, pOldState);

    // Do some initialization stuff here:

    UpdateDisplay(pContext);
}

void KPstateGraphicHint::UpdateDisplay(KPstateContext *pContext)
{
    KPstate::UpdateDisplay(pContext);

    KPmenu &menu = pContext->GetMenu();

    menu.plates[SHLD_MENUBACKGROUND].Positioniere(2,3.5,15,8.5);

    menu.plates[SHLD_LOGO].Positioniere(4,9,12,11);
    menu.plates[SHLD_LOGO].VollSichtbar();

    menu.labels[T_HINWEIS].Positioniere(8,8,1,A_MITTE);
    menu.labels[T_HINWEIS].VollSichtbar();

    menu.labels[T_HINWEIS1].Positioniere(8,6.0,0.7f,A_MITTE);
    menu.labels[T_HINWEIS2].Positioniere(8,5.5,0.7f,A_MITTE);
    if (menu.labels.find(T_HINWEIS3) != menu.labels.end())
    {
        menu.labels[T_HINWEIS3].Positioniere(8,5.0,0.7f,A_MITTE);
    }

    menu.labels[T_OK].Positioniere(8,1,1,A_MITTE);
    menu.labels[T_OK].SetzeSignal(S_BACK);

    StartAnimation();
}

void KPstateGraphicHint::KeyPressed (KPstateContext *pContext,
                                     unsigned char key, int x, int y)
{
    CHECK_DEFAULT_KEY_PRESSED(pContext, key, x, y);
}


void  KPstateGraphicHint::MouseClick (KPstateContext *pContext,
                                      tMouseButton button, tMouseEvent event,
                                      int x, int y)
{
    int Signal = KPstate::EvaluateMouseClick(pContext, button, event, x, y);

    switch (Signal)
    {
        case S_BACK:
            pContext->ChangeState(KPState_Settings);
    }
}

tKPMenuState KPstateGraphicHint::ESCKeyAction (KPstateContext *)
{
    return KPState_Settings;
}
