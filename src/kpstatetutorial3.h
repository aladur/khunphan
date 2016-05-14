/*
    kpstatetutorial3.h


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

#ifndef KPSTATETUTORIAL3_H
#define KPSTATETUTORIAL3_H

#include "stdafx.h"
#include "kpstatemovetoken.h"

enum tTutState
{
    KP_TUT_STARTED      = 33,
    KP_TUT_NO_TOKEN     = 34,
    KP_TUT_RIGHT_TOKEN  = 35,
    KP_TUT_NOMOV_TOKEN  = 36,
    KP_TUT_WRONG_TOKEN  = 37,
    KP_TUT_KLICKED_ONLY = 38
};

class KPstateContext;

class KPstateTutorial3 : public KPstateMoveToken
{
public:
    KPstateTutorial3();
    tKPMenuState GetId() const override
    {
        return KPState_Tutorial3;
    };
    void Initialize(KPstateContext *pContext,
                    tKPMenuState previousStateId) override;
    void MouseClick(KPstateContext *pContext, MouseButton button,
                    MouseButtonEvent event, int x, int y) override;
    void UpdateDisplay(KPstateContext *pContext) const override;
    tKPMenuState ESCKeyAction(KPstateContext *) const override;

protected:
    virtual int GetTextOffset() const;
    virtual tKPTokenID GetEmphasizedTokenId() const;
    virtual void InitializeBoardWithTokens(KPstateContext *pContext) const;
    virtual void ContinueWithNextState(KPstateContext *pContext,
                                       bool valid = true) const;
    void HookAfterTokenMoved(KPstateContext *pContext, tKPTokenID token,
                             MoveToken direction,
                             bool successfullyMoved) override;
    void PlayAudioForInitialize(KPstateContext *) const override;

    tTutState tutorialState;
};

#endif
