/*
    kpstate.h

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

#ifndef KPSTATE_H
#define KPSTATE_H

#include "KPstateContext.h"
#include "language.h"
#include "misc1.h"


enum tKPMenuState
{
    KPState_Invalid         = 0,
    KPState_StartUp         = 1,
    KPState_Game            = 2,
    KPState_SelectLanguage  = 3,
    KPState_MainMenu        = 4,
    KPState_Finish          = 5,
    KPState_Settings        = 6,
    KPState_ControlSettings = 7,
    KPState_GraphicSettings = 8,
    KPState_GraphicHint     = 9,
    KPState_AudioSettings   = 10,
    KPState_Help            = 11,
    KPState_KeyboardHelp    = 12,
    KPState_GameSolved      = 13,
    KPState_Tutorial1       = 14,
    KPState_Tutorial2       = 15,
    KPState_Tutorial3       = 16,
    KPState_Tutorial4       = 17,
    KPState_ScoreList       = 18,
    KPState_LightTest       = 19,
    KPState_Shutdown        = 99
};

#define CHECK_DEFAULT_KEY_PRESSED(p1, p2, p3, p4)                 \
  {                                                               \
    tKPMenuState s = KPstate::DefaultKeyPressed (p1, p2, p3, p4); \
    if (s != KPState_Invalid) {                                   \
      p1->ChangeState(s);                                         \
      return;                                                     \
    }                                                             \
  }

class KPstate
{
public:
    KPstate();
    virtual ~KPstate();

    virtual tKPMenuState GetId() const
    {
        return KPState_Invalid;
    };
    virtual void Initialize(KPstateContext *pContext, const KPstate *pOldState);
    virtual void Update(KPstateContext *pContext, int factor);
    virtual void Draw(KPstateContext *pContext);
    virtual void MouseClick           (KPstateContext *pContext,
                                       tMouseButton button, tMouseEvent event,
                                       int x, int y);
    virtual void MouseMotion          (KPstateContext *pContext, int x, int y);
    virtual void KeyPressed           (KPstateContext *pContext,
                                       unsigned char key, int x, int y);
    virtual void KeyReleased          (KPstateContext *pContext,
                                       unsigned char key, int x, int y);
    virtual tKPMenuState ESCKeyAction (KPstateContext *pContext);

protected:
    virtual void HookAfterAnimationFinished(KPstateContext *pContext);
    virtual void UpdateDisplay(KPstateContext *pContext);
    virtual void PlayAudioForInitialize(KPstateContext *pContext);

    int  EvaluateMouseClick(KPstateContext *pContext, tMouseButton button,
                            tMouseEvent event, int x, int y);
    bool EvaluateKeyPressed (KPstateContext *pContext, unsigned char key,
                             int x, int y);
    tKPMenuState DefaultKeyPressed(KPstateContext *pContext, unsigned char key,
                                   int x, int y);
    void ChangeState( KPstateContext *pContext, tKPMenuState stateID );
    void StartAnimation();

    int  AnimationTime;
    bool InAnimation;
    bool menuLocked;
    tKPMenuState oldStateId;
};

inline void KPstate::ChangeState( KPstateContext *pContext,
                                  tKPMenuState stateID )
{
    if (pContext)
    {
        pContext->ChangeState(stateID);
    }
    // No more statements should follow because
    // this object already is deleted!
}

#endif
