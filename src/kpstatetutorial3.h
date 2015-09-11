/*
    kpstatetutorial3.h


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

#ifndef KPSTATETUTORIAL3_H
#define KPSTATETUTORIAL3_H

#include "kpstatemovetoken.h"
#include "misc1.h"

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
    tKPMenuState GetId() const
    {
        return KPState_Tutorial3;
    };
    void Initialize (KPstateContext *pContext, const KPstate *pOldState);
    void MouseClick (KPstateContext *pContext, tMouseButton button,
                     tMouseEvent event, int x, int y);
    void KeyPressed (KPstateContext *pContext, unsigned char key, int x, int y);
    void UpdateDisplay(KPstateContext *pContext);
    tKPMenuState ESCKeyAction (KPstateContext *);

protected:
    virtual int  GetTextOffset();
    virtual tKPTokenID GetEmphasizedTokenId();
    virtual void InitializeBoardWithTokens(KPstateContext *pContext);
    virtual void ContinueWithNextState(KPstateContext *pContext,
                                       bool valid = true);
    void HookAfterTokenMoved(KPstateContext *pContext, tKPTokenID token,
                             tKPDirection direction, bool successfullyMoved);
    void PlayAudioForInitialize(KPstateContext *pContext);

    tTutState tutorialState;
};

#endif
