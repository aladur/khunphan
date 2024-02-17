/*
    kpstate.h

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

#ifndef KPSTATE_H
#define KPSTATE_H

#include "stdafx.h"
#include "KPstateContext.h"
#include "language.h"
#include "kpstates.h"
#include "kpsignals.h"


class KPstate
{
public:
    KPstate();
    virtual ~KPstate();

    virtual StateId GetId() const
    {
        return StateId::Invalid;
    };
    virtual void Initialize(KPstateContext *pContext,
                            StateId previousStateId);
    virtual void AnimateAll(KPstateContext *pContext,
                            unsigned int duration) const;
    virtual void Animate(KPstateContext *pContext, unsigned int duration) const;
    virtual void Draw(KPstateContext *pContext) const;
    virtual void MouseClick(KPstateContext *pContext,
                            MouseButton button, MouseButtonEvent event,
                            int x, int y);
    virtual void KeyPressed(KPstateContext *pContext,
                            unsigned char key, int x, int y) const;
    virtual StateId ESCKeyAction(KPstateContext *pContext) const;

protected:
    virtual void HookAfterAnimationFinished(KPstateContext *pContext) const;
    virtual void UpdateDisplay(KPstateContext *pContext) const;
    virtual void PlayAudioForInitialize(KPstateContext *pContext) const;

    Signal EvaluateMouseClick(KPstateContext *pContext, MouseButton button,
                            MouseButtonEvent event, int x, int y);
    bool EvaluateKeyPressed(KPstateContext *pContext, unsigned char key,
                            int x, int y) const;
    StateId DefaultKeyPressed(KPstateContext *pContext, unsigned char key,
                                   int x, int y) const;
    void ChangeState(KPstateContext *pContext, StateId stateID) const;
    void StartAnimation(KPstateContext *pContext) const;
    inline static Lbl GetLabelId(Lbl base, std::size_t offset)
    {
        const auto index = static_cast<std::size_t>(base);

        return static_cast<Lbl>(index + offset);
    }
};

inline void KPstate::ChangeState(KPstateContext *pContext,
                                 StateId stateID) const
{
    if (pContext)
    {
        pContext->ChangeState(stateID);
    }

    // No more statements should follow because
    // the this pointer is already deleted!
}

#endif
