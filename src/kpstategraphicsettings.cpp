/*
    kpstategraphicsettings.cpp

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

#include "stdafx.h"
#include <algorithm>
#include <stdexcept>
#include "kpstategraphicsettings.h"
#include "kpmenu.h"
#include "kpboardGL.h"
#include "kpconfig.h"
#include "light.h"
#include "kpuibase.h"


void KPstateGraphicSettings::Initialize(KPstateContext *pContext,
                                        const KPstate *pPreviousState)
{
    KPstate::Initialize(pContext, pPreviousState);

    KPConfig &config = KPConfig::Instance();

    E_ScreenXResolution = config.ScreenXResolution;
    E_UserInterface = config.UserInterface;
    E_FullScreen = config.FullScreen;
    Quality = 0;
    std::vector<std::string> textureNameList = config.GetTextureNames();

    if (textureNameList.empty())
    {
        throw std::runtime_error("*** Error: No texture directories found");
    }
    std::sort(textureNameList.begin(), textureNameList.end());
    for (unsigned int i = 0; i < textureNameList.size(); i++)
    {
        if (pContext->GetMenu().labels.find(T_TEXTURE1 + i) ==
            pContext->GetMenu().labels.end())
        {
            std::stringstream message;

            message << "*** Error: Not enough Language entries for "
                    << textureNameList.size()
                    << " Textures";
            throw std::runtime_error(message.str());
        }
    }

    UpdateDisplay(pContext);
}

void KPstateGraphicSettings::UpdateQuality(KPstateContext *pContext)
{
    Quality = GetCurrentQuality(pContext);
}

int KPstateGraphicSettings::GetCurrentQuality(KPstateContext *pContext)
{
    KPConfig &config = KPConfig::Instance();
    bool canToggleFullScreen =
        pContext->GetUserInterface().CanToggleFullScreen();
    int currentQuality = 0;

    if (E_FullScreen==false &&
        E_ScreenXResolution==640 &&
        config.TextureSize==8 &&
        config.MenuTextureSize==2 &&
        config.Nearest==true &&
        config.LightSources==2)
    {
        currentQuality=1; // very fast
    }
    else if (E_FullScreen==false &&
             E_ScreenXResolution==800 &&
             config.TextureSize==4 &&
             config.MenuTextureSize==2 &&
             config.Nearest==true &&
             config.LightSources==2)
    {
        currentQuality=2; // fast
    }
    else if (E_FullScreen==false &&
             E_ScreenXResolution==1024 &&
             config.TextureSize==2 &&
             config.MenuTextureSize==1 &&
             config.Nearest==true &&
             config.LightSources==2)
    {
        currentQuality=3; // normal
    }
    else if (E_FullScreen==false &&
             E_ScreenXResolution==1280 &&
             config.TextureSize==1 &&
             config.MenuTextureSize==1 &&
             config.Nearest==false &&
             config.LightSources==3)
    {
        currentQuality=4; // high
    }
    else if (((canToggleFullScreen && (E_FullScreen==true)) ||
             (!canToggleFullScreen && (E_ScreenXResolution==1600))) &&
             config.TextureSize==1 &&
             config.MenuTextureSize==1 &&
             config.Nearest==false &&
             config.LightSources==3)
    {
        currentQuality=5; // very high
    }
    else
    {
        currentQuality=0; // user defined
    }

    return currentQuality;
}

void KPstateGraphicSettings::UpdateDisplay(KPstateContext *pContext) const
{
    KPstate::UpdateDisplay(pContext);

    KPConfig &config = KPConfig::Instance();
    KPmenu &menu     = pContext->GetMenu();

    int textureIndex = GetTextureIndex(config.TextureName);

    float y;
    float dy = 0.5;

    if (config.DisplayFPS)
    {
        menu.labels[T_FPS].SetPosition(0, 11.7f, 0.3f);
    }

    menu.plates[PLATE_MENUBACKGROUND].SetPosition(2, 1.7f, 14, 9.5f);

    menu.plates[PLATE_LOGO].SetPosition(4,10,12,12);
    menu.plates[PLATE_LOGO].SetFullyVisible();

    menu.labels[T_SETTINGS].SetPosition(4,9,1);
    menu.labels[T_SETTINGS].SetFullyVisible();
    menu.labels[T_GRAPHICS].SetPosition(12,9,1,A_RIGHT);
    menu.labels[T_GRAPHICS].SetFullyVisible();

    y = 8.0f;
    menu.labels[T_QUALITY].SetPosition(8,y,1,A_RIGHT);
    menu.labels[T_QUALITY].SetSignal(S_TOGGLE_QUALITY);
    y = 7.4f;
    if (E_FullScreen == 0)
    {
        menu.labels[T_RESOLUTION].SetPosition(8, y, 0.6f, A_RIGHT);
        menu.labels[T_RESOLUTION].SetSignal(S_TOGGLE_RESOLUTION);
    }
    y -= dy;
    if (pContext->GetUserInterface().CanToggleFullScreen())
    {
        menu.labels[T_SCREENMODE].SetPosition(8, y, 0.6f, A_RIGHT);
        menu.labels[T_SCREENMODE].SetSignal(S_TOGGLE_SCREENMODE);
        y -= dy;
    }
    menu.labels[T_MENUTEXTURES].SetPosition(8, y, 0.6f, A_RIGHT);
    y -= dy;
    menu.labels[T_MENUTEXTURES].SetSignal(S_TOGGLE_MENUTEXTURES);
    menu.labels[T_TEXTURES].SetPosition(8, y, 0.6f, A_RIGHT);
    y -= dy;
    menu.labels[T_TEXTURES].SetSignal(S_TOGGLE_TEXTURES);
    menu.labels[T_TEXTURENAME].SetPosition(8, y, 0.6f, A_RIGHT);
    y -= dy;
    menu.labels[T_TEXTURENAME].SetSignal(S_TOGGLE_TEXTURENAME);
    menu.labels[T_TEXTUREINTERPOL].SetPosition(8, y, 0.6f, A_RIGHT);
    y -= dy;
    menu.labels[T_TEXTUREINTERPOL].SetSignal(S_TOGGLE_TEXTUREINTERPOL);
    menu.labels[T_AMBIENTLIGHT].SetPosition(8, y, 0.6f, A_RIGHT);
    y -= dy;
    menu.labels[T_AMBIENTLIGHT].SetSignal(S_TOGGLE_AMBIENT_LIGHT);
    menu.labels[T_ILLUMINATION].SetPosition(8, y, 0.6f, A_RIGHT);
    y -= dy;
    menu.labels[T_ILLUMINATION].SetSignal(S_TOGGLE_LAMPS);
#if defined(HAVE_LIBGLUT) || defined(HAVE_LIBOPENGLUT)
    menu.labels[T_USERINTERFACE].SetPosition(8, y, 0.6f, A_RIGHT);
    y -= dy;
    menu.labels[T_USERINTERFACE].SetSignal(S_TOGGLE_USER_INTERFACE);
#endif
    menu.labels[T_FRAMERATE].SetPosition(8, y, 0.6f, A_RIGHT);
    y -= dy;
    menu.labels[T_FRAMERATE].SetSignal(S_TOGGLE_FPS);

    y = 8.0;
    switch (Quality)
    {
        case 1:
        {
            menu.labels[T_Q_VERYFAST].SetPosition(8.2f, y, 1);
            menu.labels[T_Q_VERYFAST].SetSignal(S_TOGGLE_QUALITY);
        }
        break;
        case 2:
        {
            menu.labels[T_Q_FAST].SetPosition(8.2f, y, 1);
            menu.labels[T_Q_FAST].SetSignal(S_TOGGLE_QUALITY);
        }
        break;
        case 3:
        {
            menu.labels[T_Q_NORMAL].SetPosition(8.2f, y, 1);
            menu.labels[T_Q_NORMAL].SetSignal(S_TOGGLE_QUALITY);
        }
        break;
        case 4:
        {
            menu.labels[T_Q_HIGH].SetPosition(8.2f, y, 1);
            menu.labels[T_Q_HIGH].SetSignal(S_TOGGLE_QUALITY);
        }
        break;
        case 5:
        {
            menu.labels[T_Q_VERYHIGH].SetPosition(8.2f, y, 1);
            menu.labels[T_Q_VERYHIGH].SetSignal(S_TOGGLE_QUALITY);
        }
        break;
        default:
        {
            menu.labels[T_Q_USER].SetPosition(8.2f, y, 1);
            menu.labels[T_Q_USER].SetSignal(S_TOGGLE_QUALITY);
        }
        break;
    }

    y = 7.4f;
    if (!E_FullScreen)
    {
        switch (E_ScreenXResolution)
        {
            case 640:
            {
                menu.labels[T_RES_640x480].SetPosition(8.2f, y, 0.6f);
                menu.labels[T_RES_640x480].SetSignal(S_TOGGLE_RESOLUTION);
            }
            break;
            case 1024:
            {
                menu.labels[T_RES_1024x768].SetPosition(8.2f, y, 0.6f);
                menu.labels[T_RES_1024x768].SetSignal(S_TOGGLE_RESOLUTION);
            }
            break;
            case 1280:
            {
                menu.labels[T_RES_1280x960].SetPosition(8.2f, y, 0.6f);
                menu.labels[T_RES_1280x960].SetSignal(S_TOGGLE_RESOLUTION);
            }
            break;
            case 1600:
            {
                menu.labels[T_RES_1600x1200].SetPosition(8.2f, y, 0.6f);
                menu.labels[T_RES_1600x1200].SetSignal(S_TOGGLE_RESOLUTION);
            }
            break;
            default:
            {
                menu.labels[T_RES_800x600].SetPosition(8.2f, y, 0.6f);
                menu.labels[T_RES_800x600].SetSignal(S_TOGGLE_RESOLUTION);
            }
            break;
        }
    }

    if (pContext->GetUserInterface().CanToggleFullScreen())
    {
        y -= dy;
        if (E_FullScreen)
        {
            menu.labels[T_FULLSCREEN].SetPosition(8.2f, y, 0.6f);
            menu.labels[T_FULLSCREEN].SetSignal(S_TOGGLE_SCREENMODE);
        }
        else
        {
            menu.labels[T_WINDOW].SetPosition(8.2f, y, 0.6f);
            menu.labels[T_WINDOW].SetSignal(S_TOGGLE_SCREENMODE);
        }
    }

    y -= dy;
    switch (config.MenuTextureSize)
    {
        case 1:
        {
            menu.labels[T_MT_HIGH].SetPosition(8.2f, y, 0.6f);
            menu.labels[T_MT_HIGH].SetSignal(S_TOGGLE_MENUTEXTURES);
        }
        break;
        case 2:
        {
            menu.labels[T_MT_NORMAL].SetPosition(8.2f, y, 0.6f);
            menu.labels[T_MT_NORMAL].SetSignal(S_TOGGLE_MENUTEXTURES);
        }
        break;
    }

    y -= dy;
    switch (config.TextureSize)
    {
        case 1:
        {
            menu.labels[T_T_HIGH].SetPosition(8.2f, y, 0.6f);
            menu.labels[T_T_HIGH].SetSignal(S_TOGGLE_TEXTURES);
        }
        break;
        case 2:
        {
            menu.labels[T_T_NORMAL].SetPosition(8.2f, y, 0.6f);
            menu.labels[T_T_NORMAL].SetSignal(S_TOGGLE_TEXTURES);
        }
        break;
        case 4:
        {
            menu.labels[T_T_LOW].SetPosition(8.2f, y, 0.6f);
            menu.labels[T_T_LOW].SetSignal(S_TOGGLE_TEXTURES);
        }
        break;
        case 8:
        {
            menu.labels[T_T_VERYLOW].SetPosition(8.2f, y, 0.6f);
            menu.labels[T_T_VERYLOW].SetSignal(S_TOGGLE_TEXTURES);
        }
        break;
    }

    y -= dy;
    menu.labels[T_TEXTURE1 + textureIndex].SetPosition(8.2f, y, 0.6f);
    menu.labels[T_TEXTURE1 + textureIndex].SetSignal(S_TOGGLE_TEXTURENAME);

    y -= dy;
    if (config.Nearest)
    {
        menu.labels[T_TI_OFF].SetPosition(8.2f, y, 0.6f);
        menu.labels[T_TI_OFF].SetSignal(S_TOGGLE_TEXTUREINTERPOL);
    }
    else
    {
        menu.labels[T_TI_ON].SetPosition(8.2f, y, 0.6f);
        menu.labels[T_TI_ON].SetSignal(S_TOGGLE_TEXTUREINTERPOL);
    }

    y -= dy;
    if (config.AmbientLight)
    {
        menu.labels[T_AL_ON].SetPosition(8.2f, y, 0.6f);
        menu.labels[T_AL_ON].SetSignal(S_TOGGLE_AMBIENT_LIGHT);
    }
    else
    {
        menu.labels[T_AL_OFF].SetPosition(8.2f, y, 0.6f);
        menu.labels[T_AL_OFF].SetSignal(S_TOGGLE_AMBIENT_LIGHT);

    }

    y -= dy;
    switch (config.LightSources)
    {
        case 1:
        {
            menu.labels[T_ILLUM1].SetPosition(8.2f, y, 0.6f);
            menu.labels[T_ILLUM1].SetSignal(S_TOGGLE_LAMPS);
        }
        break;
        case 2:
        {
            menu.labels[T_ILLUM2].SetPosition(8.2f, y, 0.6f);
            menu.labels[T_ILLUM2].SetSignal(S_TOGGLE_LAMPS);
        }
        break;
        case 3:
        {
            menu.labels[T_ILLUM3].SetPosition(8.2f, y, 0.6f);
            menu.labels[T_ILLUM3].SetSignal(S_TOGGLE_LAMPS);
        }
        break;
    }

#if defined(HAVE_LIBGLUT) || defined(HAVE_LIBOPENGLUT)
    y -= dy;
    switch (E_UserInterface)
    {
        case 0:
        {
            menu.labels[T_USERINTERFACE0].SetPosition(8.2f, y, 0.6f);
            menu.labels[T_USERINTERFACE0].SetSignal(S_TOGGLE_USER_INTERFACE);
        }
        break;
        case 1:
        {
            menu.labels[T_USERINTERFACE1].SetPosition(8.2f, y, 0.6f);
            menu.labels[T_USERINTERFACE1].SetSignal(S_TOGGLE_USER_INTERFACE);
        }
        break;
    }
#endif

    y -= dy;
    if (config.DisplayFPS)
    {
        menu.labels[T_FR_ON].SetPosition(8.2f, y, 0.6f);
        menu.labels[T_FR_ON].SetSignal(S_TOGGLE_FPS);
    }
    else
    {
        menu.labels[T_FR_OFF].SetPosition(8.2f, y, 0.6f);
        menu.labels[T_FR_OFF].SetSignal(S_TOGGLE_FPS);
    }

    menu.labels[T_BACK].SetPosition(8, 0.7f, 1 ,A_CENTERED);
    menu.labels[T_BACK].SetSignal(S_BACK);

    StartAnimation(pContext);
}

void KPstateGraphicSettings::MouseClick(KPstateContext *pContext,
                                        tMouseButton button, tMouseEvent event,
                                        int x, int y)
{
    int Signal = KPstate::EvaluateMouseClick(pContext, button, event, x, y);

    switch (Signal)
    {
        case S_TOGGLE_FPS:
            ToggleFPS(pContext);
            UpdateDisplay(pContext);
            break;
        case S_TOGGLE_LAMPS:
            ToggleLamps(pContext);
            UpdateDisplay(pContext);
            break;
        case S_TOGGLE_SCREENMODE:
            ToggleScreenMode(pContext);
            UpdateDisplay(pContext);
            break;
        case S_TOGGLE_RESOLUTION:
            ToggleResolution(pContext);
            UpdateDisplay(pContext);
            break;
        case S_TOGGLE_TEXTUREINTERPOL:
            ToggleTextureInterpolation(pContext);
            UpdateDisplay(pContext);
            break;
        case S_TOGGLE_TEXTURES:
            ToggleTextures(pContext);
            UpdateDisplay(pContext);
            break;
        case S_TOGGLE_MENUTEXTURES:
            ToggleMenuTextures(pContext);
            UpdateDisplay(pContext);
            break;
        case S_TOGGLE_TEXTURENAME:
            ToggleTextureName(pContext);
            UpdateDisplay(pContext);
            break;
        case S_TOGGLE_AMBIENT_LIGHT:
            ToggleAmbientLight(pContext);
            UpdateDisplay(pContext);
            break;
#if defined(HAVE_LIBGLUT) || defined(HAVE_LIBOPENGLUT)
        case S_TOGGLE_USER_INTERFACE:
            ToggleUserInterface(pContext);
            UpdateDisplay(pContext);
            break;
#endif
        case S_TOGGLE_QUALITY:
            ToggleQuality(pContext);
            UpdateDisplay(pContext);
            break;

        case S_BACK:
        {
            tKPMenuState newState = SaveChanges(pContext);
            pContext->ChangeState(newState);
        }
    }
}

tKPMenuState KPstateGraphicSettings::ESCKeyAction(
                                         KPstateContext *pContext) const
{
    tKPMenuState newState;

    newState = SaveChanges(pContext);
    return newState;
}

/////////////////////////////////////////////////////////////////////
// Take over changes and change state
/////////////////////////////////////////////////////////////////////

tKPMenuState KPstateGraphicSettings::SaveChanges(KPstateContext *pContext) const
{
    bool ChangeNeedsToolRestart = false;
    KPConfig &config = KPConfig::Instance();
    bool canChangeWindowSize = pContext->GetUserInterface().
                                   CanChangeWindowSize();
    bool canToggleFullScreen = pContext->GetUserInterface().
                                   CanToggleFullScreen();

    if ((!canToggleFullScreen &&
         (config.FullScreen != E_FullScreen)) ||
        (!canChangeWindowSize &&
         (config.ScreenXResolution != E_ScreenXResolution)) ||
        config.UserInterface != E_UserInterface)
    {
        ChangeNeedsToolRestart = true;
    }

    LOG1("Changed Graphic Settings");
    LOG2("  UserInterface :        ",
         KPConfig::GetUserInterfaceName(E_UserInterface));
    LOG2("  TextureName :          ", config.TextureName);
    LOG2("  TextureSize :          ", config.TextureSize);
    LOG2("  MenuTextureSize :      ", config.MenuTextureSize);
    LOG2("  TextureInterpolation : ", ON_OFF(config.Nearest));
    LOG2("  ScreenXResolution :    ", E_ScreenXResolution);
    LOG2("  ColorDepth :           ", config.ColorDepth);
    LOG2("  AmbientLight :         ", ON_OFF(config.AmbientLight));
    LOG2("  LightSources :         ", config.LightSources);
    LOG2("  FullScreen :           ", ON_OFF(E_FullScreen));

    config.FullScreen        = E_FullScreen;
    config.ScreenXResolution = E_ScreenXResolution;
    config.UserInterface     = E_UserInterface;

    config.WriteToFile();

    if (ChangeNeedsToolRestart)
    {
        return KPState_GraphicHint;
    }
    else
    {
        return pContext->GetPreviousState();
    }
}

/////////////////////////////////////////////////////////////////////
// Toggle settings
/////////////////////////////////////////////////////////////////////

void KPstateGraphicSettings::ToggleFPS(KPstateContext *pContext) const
{
    pContext->GetUserInterface().PlayAudio(KP_SND_CHANGESETTING);
    KPConfig::Instance().DisplayFPS = !KPConfig::Instance().DisplayFPS;
}

void KPstateGraphicSettings::ToggleAmbientLight(KPstateContext *pContext) const
{
    KPConfig &config = KPConfig::Instance();

    pContext->GetUserInterface().PlayAudio(KP_SND_CHANGESETTING);
    KPConfig::Instance().AmbientLight = !config.AmbientLight;
    pContext->GetLight().Update(config.AmbientLight,
                                config.LightSources);
}

void KPstateGraphicSettings::ToggleLamps(KPstateContext *pContext)
{
    KPConfig &config = KPConfig::Instance();

    pContext->GetUserInterface().PlayAudio(KP_SND_CHANGESETTING);
    switch (config.LightSources)
    {
        case 1:
            config.LightSources = 2;
            break;
        case 2:
            config.LightSources = 3;
            break;
        case 3:
            config.LightSources = 1;
            break;
        default:
            config.LightSources = 1;
            break;
    }
    pContext->GetLight().Update(config.AmbientLight,
                                config.LightSources);
    UpdateQuality(pContext);
}

void KPstateGraphicSettings::ToggleTextures(KPstateContext *pContext)
{
    KPConfig &config = KPConfig::Instance();

    pContext->GetUserInterface().PlayAudio(KP_SND_CHANGESETTING);
    switch (config.TextureSize)
    {
        case 1:
            config.TextureSize  = 2;
            break;
        case 2:
            config.TextureSize  = 4;
            break;
        case 4:
            config.TextureSize  = 8;
            break;
        case 8:
            config.TextureSize  = 1;
            break;
        default:
            config.TextureSize = 1;
            break;
    }
    pContext->GetBoardView().InitializeTextures(
        config.TextureName.c_str(),
        config.TextureSize,
        config.Nearest );
    UpdateQuality(pContext);
}

void KPstateGraphicSettings::ToggleTextureName(KPstateContext *pContext) const
{
    KPConfig &config = KPConfig::Instance();
    std::vector<std::string> textureNameList = config.GetTextureNames();

    std::sort(textureNameList.begin(), textureNameList.end());

    if (textureNameList.size() > 1)
    {
        std::vector<std::string>::const_iterator it;

        it = std::find(textureNameList.begin(), textureNameList.end(),
                       config.TextureName);
        if (it == textureNameList.end())
        {
            it = textureNameList.begin();
        }
        else
        {
            ++it;
            if (it == textureNameList.end())
            {
                it = textureNameList.begin();
            }
        }
        config.TextureName = *it;
        pContext->GetBoardView().InitializeTextures(
            config.TextureName.c_str(),
            config.TextureSize,
            config.Nearest,
            false );

        pContext->GetMenu().Update( config.TextureName,
                                    config.MenuTextureSize,
                                    config.Nearest);
        pContext->GetUserInterface().InitializeAudio(
            config.TextureName.c_str(), true);

        pContext->GetUserInterface().PlayAudio(KP_SND_CHANGESETTING);
    }
}

void KPstateGraphicSettings::ToggleScreenMode(KPstateContext *pContext)
{
    E_FullScreen = !E_FullScreen;
    pContext->GetUserInterface().PlayAudio(KP_SND_CHANGESETTING);
    if (pContext->GetUserInterface().CanToggleFullScreen())
    {
        pContext->GetUserInterface().SetWindowMode(E_FullScreen);
    }
    // We also have to set the window size here
    if (!E_FullScreen && pContext->GetUserInterface().CanChangeWindowSize())
    {
        pContext->GetUserInterface().SetWindowSize(E_ScreenXResolution,
                             (E_ScreenXResolution * 3) / 4);
    }
    UpdateQuality(pContext);
}

void KPstateGraphicSettings::ToggleResolution(KPstateContext *pContext)
{
    pContext->GetUserInterface().PlayAudio(KP_SND_CHANGESETTING);
    switch (E_ScreenXResolution)
    {
        case 640:
            E_ScreenXResolution = 800;
            break;
        case 800:
            E_ScreenXResolution = 1024;
            break;
        case 1024:
            E_ScreenXResolution = 1280;
            break;
        case 1280:
            E_ScreenXResolution = 1600;
            break;
        case 1600:
            E_ScreenXResolution = 640;
            break;
        default:
            E_ScreenXResolution = 800;
            break;
    }

    pContext->GetUserInterface().PlayAudio(KP_SND_CHANGESETTING);

    if (pContext->GetUserInterface().CanChangeWindowSize())
    {
        pContext->GetUserInterface().SetWindowSize(E_ScreenXResolution,
                             (E_ScreenXResolution * 3) / 4);
    }
    UpdateQuality(pContext);
}

void KPstateGraphicSettings::ToggleUserInterface(KPstateContext *pContext)
{
    pContext->GetUserInterface().PlayAudio(KP_SND_CHANGESETTING);
    switch (E_UserInterface)
    {
        case 0:
            E_UserInterface = 1;
            break;
        case 1:
            E_UserInterface = 0;
            break;
        default:
            E_UserInterface = 0;
            break;
    }
}

void KPstateGraphicSettings::ToggleMenuTextures(KPstateContext *pContext)
{
    KPConfig &config = KPConfig::Instance();

    pContext->GetUserInterface().PlayAudio(KP_SND_CHANGESETTING);
    switch (config.MenuTextureSize)
    {
        case 1:
            config.MenuTextureSize = 2;
            break;
        case 2:
            config.MenuTextureSize = 1;
            break;
        default:
            config.MenuTextureSize = 1;
            break;
    }
    pContext->GetMenu().Update( config.TextureName,
                                config.MenuTextureSize,
                                config.Nearest );
    UpdateQuality(pContext);
}

void KPstateGraphicSettings::ToggleTextureInterpolation(
    KPstateContext *pContext) const
{
    KPConfig &config = KPConfig::Instance();

    pContext->GetUserInterface().PlayAudio(KP_SND_CHANGESETTING);
    config.Nearest = !config.Nearest;
    pContext->GetMenu().Update( config.TextureName,
                                config.MenuTextureSize,
                                config.Nearest );
    pContext->GetBoardView().InitializeTextures(
        config.TextureName.c_str(),
        config.TextureSize,
        config.Nearest );
}

void KPstateGraphicSettings::ToggleQuality(KPstateContext *pContext)
{
    KPConfig &config = KPConfig::Instance();
    bool E_FullScreen_before = E_FullScreen;
    int E_ScreenXResolution_before = E_ScreenXResolution;

    pContext->GetUserInterface().PlayAudio(KP_SND_CHANGESETTING);
    UpdateQuality(pContext);
    Quality--;
    if (Quality <= 0)
    {
        Quality = 5;
    }

    switch(Quality)
     {
        case 1:
        {
            E_FullScreen=false;
            E_ScreenXResolution=640;
            config.TextureSize=8;
            config.MenuTextureSize=2;
            config.Nearest=true;
            config.LightSources=2;
        }
        break;
        case 2:
        {
            E_FullScreen=false;
            E_ScreenXResolution=800;
            config.TextureSize=4;
            config.MenuTextureSize=2;
            config.Nearest=true;
            config.LightSources=2;
        }
        break;
        case 3:
        {
            E_FullScreen=false;
            E_ScreenXResolution=1024;
            config.TextureSize=2;
            config.MenuTextureSize=1;
            config.Nearest=true;
            config.LightSources=2;
        }
        break;
        case 4:
        {
            E_FullScreen=false;
            E_ScreenXResolution=1280;
            config.TextureSize=1;
            config.MenuTextureSize=1;
            config.Nearest=false;
            config.LightSources=3;
        }
        break;
        default:
        {
            E_FullScreen = pContext->GetUserInterface().CanToggleFullScreen();
            E_ScreenXResolution=1600;
            config.TextureSize=1;
            config.MenuTextureSize=1;
            config.Nearest=false;
            config.LightSources=3;
        }
        break;
     }

    pContext->GetLight().Update(config.AmbientLight,
                                config.LightSources);
    pContext->GetMenu().Update( config.TextureName,
                                config.MenuTextureSize,
                                config.Nearest );
    pContext->GetBoardView().InitializeTextures(
        config.TextureName.c_str(),
        config.TextureSize,
        config.Nearest );
    if (E_FullScreen_before != E_FullScreen &&
        (pContext->GetUserInterface().CanToggleFullScreen()))
    {
        pContext->GetUserInterface().SetWindowMode(E_FullScreen);
    }
    if (!E_FullScreen &&
        (E_ScreenXResolution_before != E_ScreenXResolution) &&
        (pContext->GetUserInterface().CanChangeWindowSize()))
    {
        pContext->GetUserInterface().SetWindowSize(E_ScreenXResolution,
                             (E_ScreenXResolution * 3) / 4);
    }
    UpdateDisplay(pContext);
}

int KPstateGraphicSettings::GetTextureIndex(std::string &TextureName) const
{
    std::vector<std::string> textureNameList =
        KPConfig::Instance().GetTextureNames();
    int index = 0;

    std::sort(textureNameList.begin(), textureNameList.end());
    std::vector<std::string>::const_iterator it = textureNameList.begin();

    while (it != textureNameList.end() && *it != TextureName)
    {
        index++;
        ++it;
    }
    if (it == textureNameList.end())
    {
        index = 0;
        TextureName = *textureNameList.begin();
    }
    return index;
}
