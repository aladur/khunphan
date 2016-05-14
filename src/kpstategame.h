/*
    kpstategame.h

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

#ifndef KPSTATEGAME_H
#define KPSTATEGAME_H

#include "kpstatemovetoken.h"
#include "kpuibase.h"


class KPstateGame : public KPstateMoveToken
{
    typedef void (KPstateGame::*tCallbackFct)(KPstateContext *);

public:
    tKPMenuState GetId() const override
    {
        return KPState_Game;
    };
    void Initialize(KPstateContext *pContext,
                    tKPMenuState previousStateId) override;
    void MouseClick(KPstateContext *pContext, MouseButton button,
                    MouseButtonEvent event, int x, int y) override;
    void KeyPressed(KPstateContext *pContext, unsigned char key,
                    int x, int y) const override;
    void AnimateAll(KPstateContext *pContext,
                    unsigned int duration) const override;
    void UpdateDisplay(KPstateContext *pContext) const override;
    tKPMenuState ESCKeyAction(KPstateContext *) const override;

protected:
    void HookAfterAnimationFinished(KPstateContext *) const override;
    void UpdateMoveCount(KPstateContext *pContext) const;
    void Pause(KPstateContext *pContext, bool On = true) const;
    void GameIsSolved(KPstateContext *pContext) const;
    void Cheat1(KPstateContext *pContext) const;
    void SaveGameStatus(KPstateContext *pContext) const;
    void PlayAudioForInitialize(KPstateContext *) const override;
    // KPstateMoveToken interface
    void HookAfterTokenMoved(KPstateContext *pContext, tKPTokenID token,
                             tKPDirection direction,
                             bool SuccessfullyMoved) override;
};

#endif
