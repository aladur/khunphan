/*
    kpstategraphichint.cpp

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

    menu.plates[PLATE_MENUBACKGROUND].SetPosition(2,3.5,15,8.5);

    menu.plates[PLATE_LOGO].SetPosition(4,9,12,11);
    menu.plates[PLATE_LOGO].SetFullyVisible();

    menu.labels[T_HINT].SetPosition(8,8,1,A_CENTERED);
    menu.labels[T_HINT].SetFullyVisible();

    menu.labels[T_HINT1].SetPosition(8,6.0,0.7f,A_CENTERED);
    menu.labels[T_HINT1].SetPosition(8,5.5,0.7f,A_CENTERED);
    if (menu.labels.find(T_HINT3) != menu.labels.end())
    {
        menu.labels[T_HINT3].SetPosition(8,5.0,0.7f,A_CENTERED);
    }

    menu.labels[T_OK].SetPosition(8,1,1,A_CENTERED);
    menu.labels[T_OK].SetSignal(S_BACK);

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
