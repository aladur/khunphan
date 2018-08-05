/*
    kpstatecontrolsettings.h

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

#ifndef KPSTATECONTROLSETTINGS_H
#define KPSTATECONTROLSETTINGS_H

#include "kpstate.h"


class KPstateControlSettings : public KPstate
{
public:
    StateId GetId() const override
    {
        return StateId::ControlSettings;
    };
    void Initialize(KPstateContext *pContext,
                    StateId previousStateId) override;
    void MouseClick(KPstateContext *pContext, MouseButton button,
                    MouseButtonEvent event, int x, int y) override;
    void UpdateDisplay(KPstateContext *pContext) const override;
    StateId ESCKeyAction(KPstateContext *pContext) const override;

protected:
    void SaveChanges(KPstateContext *pContext) const;
    void ToggleMouseSpeed(KPstateContext *pContext);
    void ToggleSolutionHint(KPstateContext *pContext);

    float  E_MouseSpeed;
    int    E_SolutionHint;
};

#endif
