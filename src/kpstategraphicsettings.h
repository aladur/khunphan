/*
    kpstategraphicsettings.h

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

#ifndef KPSTATEGRAPHICSETTINGS_H
#define KPSTATEGRAPHICSETTINGS_H

#include <vector>
#include "kpstate.h"
#include <string>



class KPstateGraphicSettings : public KPstate
{
public:
    tKPMenuState GetId() const
    {
        return KPState_GraphicSettings;
    };
    void Initialize (KPstateContext *pContext, const KPstate *pOldState);
    void MouseClick (KPstateContext *pContext, int button, int state,
                     int x, int y);
    void KeyPressed (KPstateContext *pContext, unsigned char key, int x, int y);
    void UpdateDisplay(KPstateContext *pContext);
    tKPMenuState ESCKeyAction(KPstateContext *pContext);

protected:
    int  GetTextureIndex(std::string &TextureName);
    tKPMenuState SaveChanges(KPstateContext *pContext);
    void ToggleFPS(KPstateContext *pContext);
    void ToggleTextures(KPstateContext *pContext);
    void ToggleMenuTextures(KPstateContext *pContext);
    void ToggleScreenMode(KPstateContext *pContext);
    void ToggleResolution(KPstateContext *pContext);
    void ToggleColorDepth(KPstateContext *pContext);
    void ToggleReflections(KPstateContext *pContext);
    void ToggleShadows(KPstateContext *pContext);
    void ToggleTextureInterpolation(KPstateContext *pContext);
    void ToggleAmbientLight(KPstateContext *pContext);
    void ToggleLamps(KPstateContext *pContext);
    void ToggleUserInterface(KPstateContext *pContext);
    void ToggleQuality(KPstateContext *pContext);
    void ToggleTextureName(KPstateContext *pContext);

    int Quality;

    int  E_ScreenXResolution;
    bool E_FullScreen;
    int  E_ColorDepth;
    int  E_UserInterface;

    bool firstTexture;
    int  textureIndex;
    std::vector<std::string> textureNameList;
};

#endif
