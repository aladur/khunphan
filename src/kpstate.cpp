/*
    kpstate.cpp

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

#include "misc1.h"
#include "kpstate.h"
#include "KPstateContext.h"
#include "kpmenu.h"
#include "kpconfig.h"
#include "Kamera.h"
#include "light.h"
#include "kpboardGL.h"
#include "kpuibase.h"
#include "sprache.h"
#include "btime.h"


KPstate::KPstate() : AnimationTime(0), InAnimation(false), menuLocked(false),
    oldStateId(KPState_Invalid)
{
}

KPstate::~KPstate()
{
}

void KPstate::Initialize(KPstateContext *pContext, const KPstate *pOldState)
{
    KPmenu &menu = pContext->GetMenu();

    oldStateId = KPState_Invalid;
    if (pOldState != NULL)
    {
        oldStateId = pOldState->GetId();
    }

    // Sw: unfinished. Should be enough in UpdateDisplay
    menu.DeactivateAllLabels();
    menu.DeactivateAllTextFields();
    PlayAudioForInitialize(pContext);
}

void KPstate::UpdateDisplay(KPstateContext *pContext)
{
    KPmenu   &menu          = pContext->GetMenu();
    KPUIBase &userInterface = pContext->GetUserInterface();

    // by default deactivate all drawing primitives

    menu.DeactivateAllLabels();
    menu.DeactivateAllTextFields();
    menu.progressBar.Deactivate();

    float y = 11.7;

    if (KPConfig::Instance().DisplayFPS)
    {
        menu.TextfeldArray[T_FPS].Positioniere(0.1, y, 0.3);
    }
    y -= 0.6;

    if (menu.IsDisplayOpenGLInfo)
    {
        menu.TextfeldArray[T_GL_VENDOR  ].FormatText(NULL,
                userInterface.GetOpenGLVendor().c_str());
        menu.TextfeldArray[T_GL_RENDERER].FormatText(NULL,
                userInterface.GetOpenGLRenderer().c_str());
        menu.TextfeldArray[T_GL_VERSION ].FormatText(NULL,
                userInterface.GetOpenGLVersion().c_str());

        menu.TextfeldArray[T_GL_VENDOR  ].Positioniere(0.1, y, 0.3);
        y -= 0.3;
        menu.TextfeldArray[T_GL_RENDERER].Positioniere(0.1, y, 0.3);
        y -= 0.3;
        menu.TextfeldArray[T_GL_VERSION ].Positioniere(0.1, y, 0.3);
    }
}

void KPstate::Update(KPstateContext *pContext, int factor)
{
    KPmenu &menu = pContext->GetMenu();


    if (InAnimation)
    {
        AnimationTime += factor;
        if (AnimationTime >= TOTAL_ANIMATIONTIME)
        {
            AnimationTime = TOTAL_ANIMATIONTIME;
            InAnimation = false;
            HookAfterAnimationFinished(pContext);
        }

        tSchildArray::iterator sit;

        for (sit = menu.SchildArray.begin(); sit != menu.SchildArray.end();
             ++sit)
        {
            sit->second.Animiere(factor);
        }

        tTextfeldArray::iterator tit;

        for (tit = menu.TextfeldArray.begin(); tit != menu.TextfeldArray.end();
             ++tit)
        {
            tit->second.Animiere(factor);
        }
    }

    menu.progressBar.Animate(factor);

    pContext->GetBoardView().Animate(factor);

    pContext->GetCamera().Fahrt(factor);
}

void KPstate::Draw(KPstateContext *pContext)
{
    double timeBoardView, timeMenu;
    BTime time;

    pContext->GetLight().Draw();

    pContext->GetCamera().male();

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

    tSchildArray::iterator sit;

    for (sit = menu.SchildArray.begin(); sit != menu.SchildArray.end(); ++sit)
    {
        Signal = sit->second.Maustaste(button,event,x,y,
                 pContext->GetUserInterface());

        if (Signal)
        {
            return Signal;
        }
    }

    tTextfeldArray::iterator tit;

    for (tit = menu.TextfeldArray.begin(); tit != menu.TextfeldArray.end();
         ++tit)
    {
        if (Signal)
        {
            break;
        }

        Signal = tit->second.Maustaste(button, event, x, y,
                                       pContext->GetUserInterface());
    }

    return Signal;
}

bool KPstate::EvaluateKeyPressed (KPstateContext *pContext, unsigned char key,
                                  int, int)
{
    // returns true if key has been evaluated
    KPmenu &menu = pContext->GetMenu();

    bool action = false;

    tTextfeldArray::iterator it;

    for (it = menu.TextfeldArray.begin(); it != menu.TextfeldArray.end(); ++it)
    {
        if (action)
        {
            break;
        }
        action = it->second.Zeichen(key);
    }
    return action;
}

void KPstate::StartAnimation()
{
    InAnimation   = true;
    AnimationTime = 0;
}

tKPMenuState KPstate::DefaultKeyPressed(KPstateContext *pContext,
                                        unsigned char key, int, int)
{
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
            KPConfig::Instance().DisplayFPS = !KPConfig::Instance().DisplayFPS;
            UpdateDisplay(pContext);
            break;
        case 'F' - 'A' + 1:
            KPConfig::Instance().FullScreen = !KPConfig::Instance().FullScreen;
            pContext->GetUserInterface().SetWindowMode(
                KPConfig::Instance().FullScreen != 0);
            //UpdateDisplay(pContext);
            break;
    }
    return KPState_Invalid;
}


void KPstate::HookAfterAnimationFinished(KPstateContext *)
{
    // to be reimplemented in subclass
}

tKPMenuState KPstate::ESCKeyAction (KPstateContext *)
{
    // This is the default behaviour. To be
    // reimplemented in subclass if state has
    // to be changed return the next
    // state otherwise return KPState_Invalid
    return oldStateId;
}

void KPstate::PlayAudioForInitialize(KPstateContext *pContext)
{
    pContext->GetUserInterface().PlayAudio(KP_SND_OPENMENU);
}

/////////////////////////////////////////////////////////////////////
// Event Handling
/////////////////////////////////////////////////////////////////////

void KPstate::MouseClick(KPstateContext *, tMouseButton, tMouseEvent, int, int)
{
}

void KPstate::MouseMotion(KPstateContext *, int, int)
{
}

void KPstate::KeyPressed (KPstateContext *, unsigned char, int, int)
{
}

void KPstate::KeyReleased(KPstateContext *, unsigned char, int, int)
{
}
