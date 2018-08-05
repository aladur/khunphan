/*
    kpstatetutorial3.h


    Automatic solution finder for KhunPhan game
    Copyright (C) 2001-2018  W. Schwotzer

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
#include <cstddef>
#include "kpstatemovetoken.h"


class KPstateContext;

class KPstateTutorial3 : public KPstateMoveToken
{
public:
    KPstateTutorial3();
    StateId GetId() const override
    {
        return StateId::Tutorial3;
    };
    void Initialize(KPstateContext *pContext,
                    StateId previousStateId) override;
    void MouseClick(KPstateContext *pContext, MouseButton button,
                    MouseButtonEvent event, int x, int y) override;
    void UpdateDisplay(KPstateContext *pContext) const override;
    StateId ESCKeyAction(KPstateContext *) const override;

protected:
    enum class ActionState
    {
        Started,
        MissedAnyToken,
        TokenKlickedOnly,
        TokenNotMoveable,
        WrongTokenMoved,
        RightTokenMoved
    };
    virtual Lbl GetTutorialLabelId(std::size_t offset = 0) const;
    virtual TokenId GetEmphasizedTokenId() const;
    virtual void InitializeBoardWithTokens(KPstateContext *pContext) const;
    virtual void ContinueWithNextState(KPstateContext *pContext,
                                       bool valid = true) const;
    void HookAfterTokenMoved(KPstateContext *pContext, TokenId token,
                             MoveToken direction,
                             bool successfullyMoved) override;
    void PlayAudioForInitialize(KPstateContext *) const override;

    ActionState actionState;
};

#endif
