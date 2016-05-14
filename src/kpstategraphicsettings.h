/*
    kpstategraphicsettings.h

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

#ifndef KPSTATEGRAPHICSETTINGS_H
#define KPSTATEGRAPHICSETTINGS_H

#include <vector>
#include "kpstate.h"
#include <string>



class KPstateGraphicSettings : public KPstate
{
public:
    KPstateGraphicSettings();
    tKPMenuState GetId() const override
    {
        return KPState_GraphicSettings;
    };
    void Initialize(KPstateContext *pContext,
                    tKPMenuState previousStateId) override;
    void MouseClick(KPstateContext *pContext, MouseButton button,
                    MouseButtonEvent event, int x, int y) override;
    void UpdateDisplay(KPstateContext *pContext) const override;
    tKPMenuState ESCKeyAction(KPstateContext *pContext) const override;

protected:
    int GetCurrentQuality(KPstateContext *pContext);
    void UpdateQuality(KPstateContext *pContext);
    int  GetTextureIndex(KPstateContext *pContext,
                         std::string &TextureName) const;
    tKPMenuState SaveChanges(KPstateContext *pContext) const;
    void ToggleFPS(KPstateContext *pContext) const;
    void ToggleTextures(KPstateContext *pContext);
    void ToggleMenuTextures(KPstateContext *pContext);
    void ToggleScreenMode(KPstateContext *pContext);
    void ToggleResolution(KPstateContext *pContext);
    void ToggleTextureInterpolation(KPstateContext *pContext) const;
    void ToggleAmbientLight(KPstateContext *pContext) const;
    void ToggleLamps(KPstateContext *pContext);
    void ToggleUserInterface(KPstateContext *pContext);
    void ToggleQuality(KPstateContext *pContext);
    void ToggleTextureName(KPstateContext *pContext) const;
    int GetWindowWidthsIndex(int width) const;

    int Quality;
    int  E_WindowWidth;
    bool E_FullScreen;
    int  E_UserInterface;
    static const int windowWidths[];
};

#endif
