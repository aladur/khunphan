/*
    kpstatemovetoken.h


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

#ifndef KPSTATEMOVETOKEN_H
#define KPSTATEMOVETOKEN_H

#include "kpstate.h"
#include "misc1.h"

class KPstateContext;

class KPstateMoveToken : public KPstate
{
public:
    KPstateMoveToken();

protected:
    bool MouseMoveToken(KPstateContext *pContext, tMouseButton button,
                        tMouseEvent event, int x, int y);
    virtual bool HookDoTheMove(KPstateContext *pContext, tKPTokenID token,
                               tKPDirection direction);
    virtual void HookAfterTokenMoved(KPstateContext *pContext, tKPTokenID token,
                                     tKPDirection direction,
                                     bool successfullyMoved);

    int mouse_x;
    int mouse_y;
};

#endif
