/*
    kpstategamesolved.h

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

#ifndef KPSTATEGAMESOLVED_H
#define KPSTATEGAMESOLVED_H

#include "kpstate.h"
#include "language.h"


class KPstateGameSolved : public KPstate
{
public:
    KPstateGameSolved();
    tKPMenuState GetId() const
    {
        return KPState_GameSolved;
    };
    void Initialize (KPstateContext *pContext, const KPstate *pOldState);
    void MouseClick (KPstateContext *pContext, tMouseButton button,
                     tMouseEvent event, int x, int y);
    void KeyPressed (KPstateContext *pContext, unsigned char key, int x, int y);
    void UpdateDisplay(KPstateContext *pContext);
    tKPMenuState ESCKeyAction (KPstateContext *);
protected:
    void SaveScoreAndChangeState(KPstateContext *pContext);
    void PlayAudioForInitialize(KPstateContext *pContext);
    bool addToScoreList;
    int  tf_index;
};

#endif
