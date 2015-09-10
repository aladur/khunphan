/*
    KPstateContext.h

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

#ifndef __kpstatecontext_h__
#define __kpstatecontext_h__

class KPstate;
class KPboardView;
class Kamera;
class Light;
class KPmenu;
class KPStatistics;
class KPUIBase;


class KPstateContext
{
public:
    virtual void ChangeState( int stateID )     = 0;
    virtual KPboardView     &GetBoardView()     = 0;
    virtual Kamera          &GetCamera()        = 0;
    virtual Light           &GetLight()         = 0;
    virtual KPmenu          &GetMenu()          = 0;
    virtual KPStatistics    &GetStatistics()    = 0;
    virtual KPUIBase        &GetUserInterface() = 0;
};
#endif
