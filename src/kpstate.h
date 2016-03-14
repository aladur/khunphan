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

#include "stdafx.h"
#include "KPstateContext.h"
#include "language.h"
#include "kpstates.h"


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
    virtual void AnimateAll(KPstateContext *pContext,
                            unsigned int duration) const;
    virtual void Animate(KPstateContext *pContext, unsigned int duration) const;
    virtual void Draw(KPstateContext *pContext) const;
    virtual void MouseClick(KPstateContext *pContext,
                            tMouseButton button, tMouseEvent event,
                            int x, int y);
    virtual void KeyPressed(KPstateContext *pContext,
                            unsigned char key, int x, int y) const;
    virtual tKPMenuState ESCKeyAction(KPstateContext *pContext) const;

protected:
    virtual void HookAfterAnimationFinished(KPstateContext *pContext) const;
    virtual void UpdateDisplay(KPstateContext *pContext) const;
    virtual void PlayAudioForInitialize(KPstateContext *pContext) const;

    int  EvaluateMouseClick(KPstateContext *pContext, tMouseButton button,
                            tMouseEvent event, int x, int y);
    bool EvaluateKeyPressed(KPstateContext *pContext, unsigned char key,
                             int x, int y) const;
    tKPMenuState DefaultKeyPressed(KPstateContext *pContext, unsigned char key,
                                   int x, int y) const;
    void ChangeState(KPstateContext *pContext, tKPMenuState stateID) const;
    void StartAnimation(KPstateContext *pContext) const;
};

inline void KPstate::ChangeState(KPstateContext *pContext,
                                 tKPMenuState stateID) const
{
    if (pContext)
    {
        pContext->ChangeState(stateID);
    }
    // No more statements should follow because
    // this object already is deleted!
}

#endif
