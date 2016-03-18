/*
    KPstateContext.h

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

#ifndef __kpstatecontext_h__
#define __kpstatecontext_h__

#include "kpstates.h"
#include "bmanualtimer.h"

class KPstate;
class KPboardView;
class Camera;
class Light;
class KPmenu;
class KPStatistics;
class KPUIBase;
class KPnodes;
class KPConfig;


class KPstateContext
{
public:
    virtual void ChangeState(tKPMenuState stateID) = 0;
    virtual tKPMenuState GetPreviousState() const = 0;
    virtual void SetPreviousState(tKPMenuState stateID) = 0;
    virtual void SetPause(bool on) = 0;
    virtual bool IsPause() = 0;
    virtual KPboardView &GetBoardView() = 0;
    virtual Camera &GetCamera() = 0;
    virtual Light &GetLight() = 0;
    virtual KPmenu &GetMenu() = 0;
    virtual KPnodes &GetNodes() = 0;
    virtual KPStatistics &GetStatistics() = 0;
    virtual KPUIBase &GetUserInterface() = 0;
    virtual BManualTimer &GetAnimationTimer() = 0;
    virtual KPConfig &GetConfig() = 0;
};
#endif
