/*
    kpstatestartup.cpp

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

#include "kpstatestartup.h"
#include "kpmenu.h"
#include "Kamera.h"
#include "light.h"
#include "kpboardGL.h"
#include "kpconfig.h"
#include "kpnode.h"
#include "sprache.h"
#include "misc1.h"

KPstateStartUp::KPstateStartUp() : fullInitialized(false)
{
}

void KPstateStartUp::Initialize(KPstateContext *pContext,
                                const KPstate *pOldState)
{
    KPstate::Initialize(pContext, pOldState);

    // Do some initialization stuff here:
    if (KPConfig::Instance().SkipProgressBar != 0)
    {
        fullInitialized = true;
    }

    if (pContext->GetMenu().TextfeldArray[T_COPYRIGHT2] == NULL)
    {
        pContext->GetMenu().TextfeldArray[T_COPYRIGHT2] = new Textfeld();
    }
    if (pContext->GetMenu().TextfeldArray[T_COPYRIGHT3] == NULL)
    {
        pContext->GetMenu().TextfeldArray[T_COPYRIGHT3] = new Textfeld();
    }
    if (pContext->GetMenu().TextfeldArray[T_COPYRIGHT5] == NULL)
    {
        pContext->GetMenu().TextfeldArray[T_COPYRIGHT5] = new Textfeld();
    }
    if (pContext->GetMenu().TextfeldArray[T_COPYRIGHT6] == NULL)
    {
        pContext->GetMenu().TextfeldArray[T_COPYRIGHT6] = new Textfeld();
    }

    pContext->GetMenu().TextfeldArray[T_COPYRIGHT2]->SetzeText(
        "© 2002-2015 Wolfgang Schwotzer");
    pContext->GetMenu().TextfeldArray[T_COPYRIGHT5]->SetzeText(
        "© 2001-2006 Nicklas Nygren");
    pContext->GetMenu().TextfeldArray[T_COPYRIGHT6]->SetzeText(
        "http://nifflas.ni2.se/");

    UpdateDisplay(pContext);

    pContext->GetCamera().SetzeRundflug(true);
}

void KPstateStartUp::UpdateDisplay(KPstateContext *pContext)
{
    KPstate::UpdateDisplay(pContext);

    KPmenu &menu = pContext->GetMenu();

    // every thing which should be permanently be visible within this
    // state should be listed here:

    menu.SchildArray[SHLD_LOGO]->Positioniere(5,9,11,11);

    if (!fullInitialized)
    {
        menu.progressBar.SetBarColor(1.0, 0.0, 0.0);
        menu.progressBar.SetPosition(8, 6, 6, 1.5, A_MITTE);
    }

    float y = 3.0f;
    menu.TextfeldArray[T_COPYRIGHT1]->Positioniere(8, y, 0.4f, A_MITTE);
    y -= 0.4f;
    menu.TextfeldArray[T_COPYRIGHT2]->Positioniere(8, y, 0.4f, A_MITTE);
    y -= 0.4f;
    menu.TextfeldArray[T_COPYRIGHT3]->Positioniere(8, y, 0.4f, A_MITTE);
    y -= 0.8f;
    menu.TextfeldArray[T_COPYRIGHT4]->Positioniere(8, y, 0.4f, A_MITTE);
    y -= 0.4f;
    menu.TextfeldArray[T_COPYRIGHT5]->Positioniere(8, y, 0.4f, A_MITTE);
    y -= 0.4f;
    menu.TextfeldArray[T_COPYRIGHT6]->Positioniere(8, y, 0.4f, A_MITTE);

    menu.TextfeldArray[T_NOWARRANTY]->Positioniere(8,    0,   0.5, A_MITTE);
    menu.TextfeldArray[T_VERSION   ]->Positioniere(0.1f, 0,   0.5);

    StartAnimation();
}

tKPMenuState KPstateStartUp::ESCKeyAction (KPstateContext *)
{
    return KPState_Shutdown;
}

void KPstateStartUp::KeyPressed (KPstateContext *pContext, unsigned char key,
                                 int x, int y)
{
    CHECK_DEFAULT_KEY_PRESSED(pContext, key, x, y);
}

void KPstateStartUp::MouseClick (KPstateContext *pContext,
                                 tMouseButton, tMouseEvent event,
                                 int, int)
{
    if (fullInitialized && event == KP_BUTTON_RELEASE)
    {
        if (KPConfig::Instance().Language)
        {
            pContext->ChangeState(KPState_MainMenu);
        }
        else
        {
            pContext->ChangeState(KPState_SelectLanguage);
        }
    }
}

void KPstateStartUp::Draw(KPstateContext *pContext)
{
    pContext->GetLight().Draw();

    pContext->GetCamera().male();

    if (fullInitialized)
    {
        pContext->GetBoardView().Draw();
    }

    pContext->GetMenu().Draw();
}

void KPstateStartUp::Update(KPstateContext *pContext, int factor)
{
    KPstate::Update(pContext, factor);

    if (!fullInitialized)
    {
        pContext->GetMenu().progressBar.SetPercentage(
            KPnode::SetSolveCountPercentFinished());
    }

    if (KPnode::IsSolveCountAvailable())
    {
        if (!fullInitialized)
        {
            fullInitialized = true;
            UpdateDisplay(pContext);
        }
    }
}

