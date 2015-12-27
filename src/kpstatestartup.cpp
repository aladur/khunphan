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
#include "camera.h"
#include "light.h"
#include "kpboardGL.h"
#include "kpconfig.h"
#include "kpnode.h"
#include "kpnodes.h"
#include "language.h"
#include "misc1.h"

KPstateStartUp::KPstateStartUp() : fullInitialized(false)
{
}

void KPstateStartUp::Initialize(KPstateContext *pContext,
                                const KPstate *pOldState)
{
    tIdToLabel::const_iterator it;

    KPstate::Initialize(pContext, pOldState);

    // Do some initialization stuff here:
    if (KPConfig::Instance().SkipProgressBar != 0)
    {
        fullInitialized = true;
    }

    it = pContext->GetMenu().labels.find(T_COPYRIGHT2);
    if (it == pContext->GetMenu().labels.end())
    {
        pContext->GetMenu().labels[T_COPYRIGHT2] =
           Label("© 2002-2015 Wolfgang Schwotzer");
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
            Label("© 2001-2006 Nicklas Nygren");
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

void KPstateStartUp::UpdateDisplay(KPstateContext *pContext)
{
    KPstate::UpdateDisplay(pContext);

    KPmenu &menu = pContext->GetMenu();

    // every thing which should be permanently be visible within this
    // state should be listed here:

    menu.plates[PLATE_LOGO].SetPosition(5,9,11,11);

    if (!fullInitialized)
    {
        menu.progressBar.SetBarColor(1.0, 0.0, 0.0);
        menu.progressBar.SetPosition(8, 6, 6, 1.5, A_CENTERED);
    }

    float y = 3.0f;
    menu.labels[T_COPYRIGHT1].SetPosition(8, y, 0.4f, A_CENTERED);
    y -= 0.4f;
    menu.labels[T_COPYRIGHT2].SetPosition(8, y, 0.4f, A_CENTERED);
    y -= 0.4f;
    menu.labels[T_COPYRIGHT3].SetPosition(8, y, 0.4f, A_CENTERED);
    y -= 0.8f;
    menu.labels[T_COPYRIGHT4].SetPosition(8, y, 0.4f, A_CENTERED);
    y -= 0.4f;
    menu.labels[T_COPYRIGHT5].SetPosition(8, y, 0.4f, A_CENTERED);
    y -= 0.4f;
    menu.labels[T_COPYRIGHT6].SetPosition(8, y, 0.4f, A_CENTERED);

    menu.labels[T_NOWARRANTY].SetPosition(8, 0, 0.5, A_CENTERED);
    menu.labels[T_VERSION].SetPosition(0.1f, 0, 0.5);

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

    pContext->GetCamera().Draw();

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
            KPnode::CalculateSolveCountPercentFinished());
    }

    if (pContext->GetNodes().IsSolveCountAvailable())
    {
        if (!fullInitialized)
        {
            fullInitialized = true;
            LOG5(std::fixed, std::setprecision(3),
                 "Time to calculate all solutions: ",
                 pContext->GetNodes().GetSolveTime() / 1000000.0, " s");
            UpdateDisplay(pContext);
        }
    }
}

