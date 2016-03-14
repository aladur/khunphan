/*
    kpstate.cpp

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


KPstate::KPstate()
{
}

KPstate::~KPstate()
{
}

void KPstate::Initialize(KPstateContext *pContext,
                         const KPstate *pPreviousState)
{
    KPmenu &menu = pContext->GetMenu();
    tKPMenuState previousStateId = KPState_Invalid;

    if (pPreviousState != NULL)
    {
        previousStateId = pPreviousState->GetId();
    }
    pContext->SetPreviousState(previousStateId);

    // Sw: unfinished. Should be enough in UpdateDisplay
    menu.FadeOutAllPlates();
    menu.FadeOutAllLabels();
    PlayAudioForInitialize(pContext);
}

void KPstate::UpdateDisplay(KPstateContext *pContext) const
{
    KPmenu   &menu          = pContext->GetMenu();
    KPUIBase &userInterface = pContext->GetUserInterface();

    // by default deactivate all drawing primitives

    menu.FadeOutAllPlates();
    menu.FadeOutAllLabels();

    float y = 11.7f;

    if (KPConfig::Instance().DisplayFPS)
    {
        menu.labels[T_FPS].SetPosition(0.1f, y, 0.3f);
    }
    y -= 0.6f;

    if (menu.IsDisplayOpenGLInfo)
    {
        menu.labels[T_GL_VENDOR].FormatText(
                1, userInterface.GetOpenGLVendor().c_str());
        menu.labels[T_GL_RENDERER].FormatText(
                1, userInterface.GetOpenGLRenderer().c_str());
        menu.labels[T_GL_VERSION].FormatText(
                1, userInterface.GetOpenGLVersion().c_str());

        menu.labels[T_GL_VENDOR].SetPosition(0.1f, y, 0.3f);
        y -= 0.3f;
        menu.labels[T_GL_RENDERER].SetPosition(0.1f, y, 0.3f);
        y -= 0.3f;
        menu.labels[T_GL_VERSION].SetPosition(0.1f, y, 0.3f);
    }
}

void KPstate::Animate(KPstateContext *pContext, unsigned int duration) const
{
    BManualTimer &animationTimer = pContext->GetAnimationTimer();

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
    double timeBoardView, timeMenu;
    BTime time;

    pContext->GetLight().Draw();

    pContext->GetCamera().Draw();

    if (KPConfig::Instance().PerformanceLog)
    {
        time.ResetRelativeTime();
    }

    pContext->GetBoardView().Draw();

    if (KPConfig::Instance().PerformanceLog)
    {
        timeBoardView = time.GetRelativeTimeUsf(true);
    }

    pContext->GetMenu().Draw();

    if (KPConfig::Instance().PerformanceLog)
    {
        timeMenu = time.GetRelativeTimeUsf(true);

        LOG9(std::fixed, std::setprecision(1), "BoardView: ",
             timeBoardView, " us Menu: ",
             timeMenu, " us Sum: ",
             timeBoardView + timeMenu, " us");
    }
}

int  KPstate::EvaluateMouseClick(KPstateContext *pContext, tMouseButton button,
                                 tMouseEvent event, int x, int y)
{
    KPmenu &menu = pContext->GetMenu();

    int Signal = 0;
    unsigned int i = 0;

    tIdToPlate::iterator sit;

    for (sit = menu.plates.begin(); sit != menu.plates.end(); ++sit)
    {
        Signal = sit->second.MouseEvent(button,event,x,y,
                                        pContext->GetUserInterface());

        if (Signal)
        {
            return Signal;
        }
    }

    tIdToLabel::iterator tit;

    for (tit = menu.labels.begin(); tit != menu.labels.end();
         ++tit)
    {
        if (Signal)
        {
            break;
        }

        Signal = tit->second.MouseEvent(button, event, x, y,
                                        pContext->GetUserInterface());
    }

    return Signal;
}

bool KPstate::EvaluateKeyPressed (KPstateContext *pContext, unsigned char key,
                                  int, int) const
{
    // returns true if key has been evaluated
    KPmenu &menu = pContext->GetMenu();

    bool done = false;

    tIdToLabel::iterator it;

    for (it = menu.labels.begin(); !done && it != menu.labels.end(); ++it)
    {
        done = it->second.AddCharacter(key);
    }

    return done;
}

void KPstate::StartAnimation(KPstateContext *pContext) const
{
    pContext->GetAnimationTimer().Restart();
}

tKPMenuState KPstate::DefaultKeyPressed(KPstateContext *pContext,
                                        unsigned char key, int, int) const
{
    KPConfig &config = KPConfig::Instance();

    // Default key handling which can be used in every state
    switch (key)
    {
        case 'Q' - 'A' + 1:
        case 27:
            return ESCKeyAction(pContext);
        case 'O' - 'A' + 1:
            pContext->GetMenu().IsDisplayOpenGLInfo =
                !pContext->GetMenu().IsDisplayOpenGLInfo;
            UpdateDisplay(pContext);
            break;
        case 'D' - 'A' + 1:
            config.DisplayFPS = !config.DisplayFPS;
            UpdateDisplay(pContext);
            break;
        case 'F' - 'A' + 1:
            config.FullScreen = !config.FullScreen;
            pContext->GetUserInterface().SetWindowMode(config.FullScreen != 0);
            break;
    }
    return KPState_Invalid;
}


void KPstate::HookAfterAnimationFinished(KPstateContext *) const
{
    // to be reimplemented in subclass
}

tKPMenuState KPstate::ESCKeyAction(KPstateContext *pContext) const
{
    // This is the default behaviour. To be
    // reimplemented in subclass if state has
    // to be changed return the next
    // state otherwise return KPState_Invalid
    return pContext->GetPreviousState();
}

void KPstate::PlayAudioForInitialize(KPstateContext *pContext) const
{
    pContext->GetUserInterface().PlayAudio(KP_SND_OPENMENU);
}

/////////////////////////////////////////////////////////////////////
// Event Handling
/////////////////////////////////////////////////////////////////////

void KPstate::MouseClick(KPstateContext *, tMouseButton, tMouseEvent, int, int)
{
}

void KPstate::KeyPressed(KPstateContext *pContext, unsigned char key,
                         int x, int y) const
{
    tKPMenuState newState = KPstate::DefaultKeyPressed(pContext, key, x, y);

    if (newState != KPState_Invalid)
    {
      pContext->ChangeState(newState);
      return;
    }
}

