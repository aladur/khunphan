/*
    kpstate.cpp

    Automatic solution finder for KhunPhan game
    Copyright (C) 2001-2024  W. Schwotzer

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
#include "kpstate.h"
#include "KPstateContext.h"
#include "kpmenu.h"
#include "kpconfig.h"
#include "camera.h"
#include "light.h"
#include "kpboardGL.h"
#include "kpuibase.h"
#include "language.h"
#include "btime.h"
#include "blogger.h"


KPstate::KPstate()
{
}

KPstate::~KPstate()
{
}

void KPstate::Initialize(KPstateContext *pContext, StateId previousStateId)
{
    pContext->SetPreviousState(previousStateId);

    PlayAudioForInitialize(pContext);
}

void KPstate::UpdateDisplay(KPstateContext *pContext) const
{
    auto &menu = pContext->GetMenu();
    KPUIBase &userInterface = pContext->GetUserInterface();

    // by default deactivate all drawing primitives
    menu.FadeOutAllPlates();
    menu.FadeOutAllLabels();

    float y = 11.7f;

    if (pContext->GetConfig().DisplayFPS)
    {
        menu.labels[Lbl::Fps].SetPosition(0.1f, y, 0.3f);
    }

    y -= 0.6f;

    if (menu.GetIsDisplayOpenGLInfo())
    {
        menu.labels[Lbl::OpenGLVendor].FormatText(
            userInterface.GetOpenGLVendor());
        menu.labels[Lbl::OpenGLRenderer].FormatText(
            userInterface.GetOpenGLRenderer());
        menu.labels[Lbl::OpenGLVersion].FormatText(
            userInterface.GetOpenGLVersion());

        menu.labels[Lbl::OpenGLVendor].SetPosition(0.1f, y, 0.3f);
        y -= 0.3f;
        menu.labels[Lbl::OpenGLRenderer].SetPosition(0.1f, y, 0.3f);
        y -= 0.3f;
        menu.labels[Lbl::OpenGLVersion].SetPosition(0.1f, y, 0.3f);
    }
}

void KPstate::Animate(KPstateContext *pContext, unsigned int duration) const
{
    auto &animationTimer = pContext->GetAnimationTimer();

    if (animationTimer.IsStarted())
    {
        if (animationTimer.Add(duration))
        {
            HookAfterAnimationFinished(pContext);
        }
    }
}

void KPstate::AnimateAll(KPstateContext *pContext, unsigned int duration) const
{
    Animate(pContext, duration);

    pContext->GetMenu().Animate(duration);
    pContext->GetBoardView().Animate(duration);
    pContext->GetCamera().Animate(duration);
}

void KPstate::Draw(KPstateContext *pContext) const
{
    auto timeBoardView = 0.0;
    auto timeMenu = 0.0;
    BTime time;

    pContext->GetCamera().Draw();

    if (pContext->GetConfig().PerformanceLog)
    {
        time.ResetRelativeTime();
    }

    pContext->GetBoardView().Draw();

    if (pContext->GetConfig().PerformanceLog)
    {
        timeBoardView = time.GetRelativeTimeUsf(true);
    }

    pContext->GetMenu().Draw();

    if (pContext->GetConfig().PerformanceLog)
    {
        timeMenu = time.GetRelativeTimeUsf(true);

        BLogger::Log(std::fixed, std::setprecision(1), "BoardView: ",
                     timeBoardView, " us Menu: ",
                     timeMenu, " us Sum: ",
                     timeBoardView + timeMenu, " us");
    }
}

Signal KPstate::EvaluateMouseClick(KPstateContext *pContext, MouseButton button,
                                   MouseButtonEvent event, int x, int y)
{
    auto &menu = pContext->GetMenu();

    auto signal = Signal::Void;

    for (auto &pitem : menu.plates)
    {
        signal = pitem.second.MouseEvent(button, event, x, y,
                                         pContext->GetUserInterface());

        if (signal != Signal::Void)
        {
            return signal;
        }
    }

    for (auto &litem : menu.labels)
    {
        if (signal != Signal::Void)
        {
            break;
        }

        signal = litem.second.MouseEvent(button, event, x, y,
                                         pContext->GetUserInterface());
    }

    return signal;
}

bool KPstate::EvaluateKeyPressed(KPstateContext *pContext, unsigned char key,
                                 int, int) const
{
    // returns true if key has been evaluated
    auto &menu = pContext->GetMenu();

    auto done = false;

    for (auto &item : menu.labels)
    {
        done = item.second.AddCharacter(key);
    }

    return done;
}

void KPstate::StartAnimation(KPstateContext *pContext) const
{
    pContext->GetAnimationTimer().Restart();
}

StateId KPstate::DefaultKeyPressed(KPstateContext *pContext,
                                   unsigned char key, int, int) const
{
    auto &config = pContext->GetConfig();

    // Default key handling which can be used in every state
    switch (key)
    {
        case 'Q' - 'A' + 1:
        case 27:
            return ESCKeyAction(pContext);

        case 'O' - 'A' + 1:
            pContext->GetMenu().InvertIsDisplayOpenGLInfo();
            UpdateDisplay(pContext);
            break;

        case 'D' - 'A' + 1:
            config.DisplayFPS = !config.DisplayFPS;
            UpdateDisplay(pContext);
            break;

        case 'F' - 'A' + 1:
            config.FullScreen = !config.FullScreen;
            pContext->GetUserInterface().SetWindowMode(config.FullScreen);
            break;
    }

    return StateId::Invalid;
}


void KPstate::HookAfterAnimationFinished(KPstateContext *) const
{
    // to be reimplemented in subclass
}

StateId KPstate::ESCKeyAction(KPstateContext *pContext) const
{
    // This is the default behaviour. To be
    // reimplemented in subclass if state has
    // to be changed return the next
    // state otherwise return StateId::Invalid
    return pContext->GetPreviousState();
}

void KPstate::PlayAudioForInitialize(KPstateContext *pContext) const
{
    pContext->GetUserInterface().PlayAudio(KPSound::OpenMenu);
}

/////////////////////////////////////////////////////////////////////
// Event Handling
/////////////////////////////////////////////////////////////////////

void KPstate::MouseClick(KPstateContext *, MouseButton, MouseButtonEvent,
                         int, int)
{
}

void KPstate::KeyPressed(KPstateContext *pContext, unsigned char key,
                         int x, int y) const
{
    auto newState = KPstate::DefaultKeyPressed(pContext, key, x, y);

    if (newState != StateId::Invalid)
    {
        pContext->ChangeState(newState);
        return;
    }
}

