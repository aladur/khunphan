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
#include "blogger.h"

const int KPstateGraphicSettings::windowWidths[] =
{
    640, 800, 1024, 1280, 1600, 1920
};

KPstateGraphicSettings::KPstateGraphicSettings() :
    Quality(0), E_WindowWidth(0),
    E_FullScreen(false), E_UserInterface(0)
{
}

void KPstateGraphicSettings::Initialize(KPstateContext *pContext,
                                        tKPMenuState previousStateId)
{
    KPstate::Initialize(pContext, previousStateId);

    auto &config = pContext->GetConfig();

    E_WindowWidth = config.ScreenXResolution;
    E_UserInterface = config.UserInterface;
    E_FullScreen = config.FullScreen;
    Quality = 0;
    auto textureNameList = config.GetTextureNames();

    if (textureNameList.empty())
    {
        throw std::runtime_error("Error: No texture directories found");
    }

    std::sort(textureNameList.begin(), textureNameList.end());

    decltype(textureNameList.size()) index;

    for (index = 0; index < textureNameList.size(); ++index)
    {
        if (pContext->GetMenu().labels.find(T_TEXTURE1 + index) ==
            pContext->GetMenu().labels.end())
        {
            std::stringstream message;

            message << "Error: Not enough Language entries for "
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
    auto &config = pContext->GetConfig();
    auto canToggleFullScreen =
        pContext->GetUserInterface().CanToggleFullScreen();
    auto currentQuality = 0;

    if (E_FullScreen == false &&
        E_WindowWidth == 640 &&
        config.TextureSize == 8 &&
        config.MenuTextureSize == 2 &&
        config.Nearest == true &&
        config.LightSources == 2)
    {
        currentQuality = 1; // very fast
    }
    else if (E_FullScreen == false &&
             E_WindowWidth == 800 &&
             config.TextureSize == 4 &&
             config.MenuTextureSize == 2 &&
             config.Nearest == true &&
             config.LightSources == 2)
    {
        currentQuality = 2; // fast
    }
    else if (E_FullScreen == false &&
             E_WindowWidth == 1024 &&
             config.TextureSize == 2 &&
             config.MenuTextureSize == 1 &&
             config.Nearest == true &&
             config.LightSources == 2)
    {
        currentQuality = 3; // normal
    }
    else if (E_FullScreen == false &&
             E_WindowWidth == 1280 &&
             config.TextureSize == 1 &&
             config.MenuTextureSize == 1 &&
             config.Nearest == false &&
             config.LightSources == 3)
    {
        currentQuality = 4; // high
    }
    else if (((canToggleFullScreen && (E_FullScreen == true)) ||
              (!canToggleFullScreen && (E_WindowWidth == 1600))) &&
             config.TextureSize == 1 &&
             config.MenuTextureSize == 1 &&
             config.Nearest == false &&
             config.LightSources == 3)
    {
        currentQuality = 5; // very high
    }
    else
    {
        currentQuality = 0; // user defined
    }

    return currentQuality;
}

void KPstateGraphicSettings::UpdateDisplay(KPstateContext *pContext) const
{
    auto &config = pContext->GetConfig();
    auto &menu = pContext->GetMenu();

    KPstate::UpdateDisplay(pContext);

    auto textureIndex = GetTextureIndex(pContext, config.TextureName);

    auto dy = 0.5f;

    if (config.DisplayFPS)
    {
        menu.labels[T_FPS].SetPosition(0, 11.7f, 0.3f);
    }

    menu.plates[PLATE_MENUBACKGROUND].SetPosition(2, 1.7f, 14, 9.5f);

    menu.plates[PLATE_LOGO].SetPosition(4, 10, 12, 12);
    menu.plates[PLATE_LOGO].SetFullyVisible();

    menu.labels[T_SETTINGS].SetPosition(4, 9, 1);
    menu.labels[T_SETTINGS].SetFullyVisible();
    menu.labels[T_GRAPHICS].SetPosition(12, 9, 1, A_RIGHT);
    menu.labels[T_GRAPHICS].SetFullyVisible();

    auto y = 8.0f;
    menu.labels[T_QUALITY].SetPosition(8, y, 1, A_RIGHT);
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
        static const int windowResolutionLabels[] =
        {
            T_RES_640x480, T_RES_800x600, T_RES_1024x768,
            T_RES_1280x960, T_RES_1600x1200, T_RES_1920x1440
        };

        auto index = GetWindowWidthsIndex(E_WindowWidth);

        if (index >= 0)
        {
            auto windowResolutionLabel = windowResolutionLabels[index];

            menu.labels[windowResolutionLabel].SetPosition(8.2f, y, 0.6f);
            menu.labels[windowResolutionLabel].SetSignal(S_TOGGLE_RESOLUTION);
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

    menu.labels[T_BACK].SetPosition(8, 0.7f, 1 , A_CENTERED);
    menu.labels[T_BACK].SetSignal(S_BACK);

    StartAnimation(pContext);
}

void KPstateGraphicSettings::MouseClick(KPstateContext *pContext,
                                        tMouseButton button, tMouseEvent event,
                                        int x, int y)
{
    auto Signal = KPstate::EvaluateMouseClick(pContext, button, event, x, y);

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
            auto newState = SaveChanges(pContext);
            pContext->ChangeState(newState);
        }
    }
}

tKPMenuState KPstateGraphicSettings::ESCKeyAction(
    KPstateContext *pContext) const
{
    auto newState = SaveChanges(pContext);
    return newState;
}

/////////////////////////////////////////////////////////////////////
// Take over changes and change state
/////////////////////////////////////////////////////////////////////

tKPMenuState KPstateGraphicSettings::SaveChanges(KPstateContext *pContext) const
{
    auto ChangeNeedsToolRestart = false;
    auto &config = pContext->GetConfig();
    auto canChangeWindowSize = pContext->GetUserInterface().
                               CanChangeWindowSize();
    auto canToggleFullScreen = pContext->GetUserInterface().
                               CanToggleFullScreen();

    if ((!canToggleFullScreen &&
         (config.FullScreen != E_FullScreen)) ||
        (!canChangeWindowSize &&
         (config.ScreenXResolution != E_WindowWidth)) ||
        config.UserInterface != E_UserInterface)
    {
        ChangeNeedsToolRestart = true;
    }

    BLogger::Log("Changed Graphic Settings");
    BLogger::Log("  UserInterface :        ",
                 KPConfig::GetUserInterfaceName(E_UserInterface));
    BLogger::Log("  TextureName :          ", config.TextureName);
    BLogger::Log("  TextureSize :          ", config.TextureSize);
    BLogger::Log("  MenuTextureSize :      ", config.MenuTextureSize);
    BLogger::Log("  TextureInterpolation : ", ON_OFF(config.Nearest));
    BLogger::Log("  ScreenXResolution :    ", E_WindowWidth);
    BLogger::Log("  ColorDepth :           ", config.ColorDepth);
    BLogger::Log("  AmbientLight :         ", ON_OFF(config.AmbientLight));
    BLogger::Log("  LightSources :         ", config.LightSources);
    BLogger::Log("  FullScreen :           ", ON_OFF(E_FullScreen));

    config.FullScreen        = E_FullScreen;
    config.ScreenXResolution = E_WindowWidth;
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
    pContext->GetConfig().DisplayFPS = !pContext->GetConfig().DisplayFPS;
}

void KPstateGraphicSettings::ToggleAmbientLight(KPstateContext *pContext) const
{
    auto &config = pContext->GetConfig();

    pContext->GetUserInterface().PlayAudio(KP_SND_CHANGESETTING);
    pContext->GetConfig().AmbientLight = !config.AmbientLight;
    pContext->GetLight().Update(config.AmbientLight,
                                config.LightSources);
}

void KPstateGraphicSettings::ToggleLamps(KPstateContext *pContext)
{
    auto &config = pContext->GetConfig();

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
    auto &config = pContext->GetConfig();

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
        config.GetDirectory(KP_TEXTURE_DIR),
        config.TextureName,
        config.TextureSize,
        config.Nearest);
    UpdateQuality(pContext);
}

void KPstateGraphicSettings::ToggleTextureName(KPstateContext *pContext) const
{
    auto &config = pContext->GetConfig();
    auto textureNameList = config.GetTextureNames();

    std::sort(textureNameList.begin(), textureNameList.end());

    if (textureNameList.size() > 1)
    {
        auto it = std::find(textureNameList.begin(), textureNameList.end(),
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
            config.GetDirectory(KP_TEXTURE_DIR),
            config.TextureName,
            config.TextureSize,
            config.Nearest,
            false);

        pContext->GetMenu().Update(config.TextureName,
                                   config.MenuTextureSize,
                                   config.Nearest);
        pContext->GetUserInterface().InitializeAudio(config.TextureName);

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
        pContext->GetUserInterface().SetWindowSize(E_WindowWidth,
                (E_WindowWidth * 3) / 4);
    }

    UpdateQuality(pContext);
}

void KPstateGraphicSettings::ToggleResolution(KPstateContext *pContext)
{
    const auto &userIfc = pContext->GetUserInterface();

    userIfc.PlayAudio(KP_SND_CHANGESETTING);

    auto index = GetWindowWidthsIndex(E_WindowWidth);

    if (index >= 0)
    {
        index = (index + 1) % (sizeof(windowWidths) / sizeof(windowWidths[0]));
    }
    else
    {
        index = 0;
    }

    E_WindowWidth = windowWidths[index];

    if (!userIfc.IsWindowResolutionSupported(E_WindowWidth,
            (E_WindowWidth * 3 / 4)))
    {
        E_WindowWidth = windowWidths[0];
    }

    if (userIfc.CanChangeWindowSize())
    {
        userIfc.SetWindowSize(E_WindowWidth, (E_WindowWidth * 3) / 4);
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
    auto &config = pContext->GetConfig();

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

    pContext->GetMenu().Update(config.TextureName,
                               config.MenuTextureSize,
                               config.Nearest);
    UpdateQuality(pContext);
}

void KPstateGraphicSettings::ToggleTextureInterpolation(
    KPstateContext *pContext) const
{
    auto &config = pContext->GetConfig();

    pContext->GetUserInterface().PlayAudio(KP_SND_CHANGESETTING);
    config.Nearest = !config.Nearest;
    pContext->GetMenu().Update(config.TextureName,
                               config.MenuTextureSize,
                               config.Nearest);
    pContext->GetBoardView().InitializeTextures(
        config.GetDirectory(KP_TEXTURE_DIR),
        config.TextureName,
        config.TextureSize,
        config.Nearest);
}

void KPstateGraphicSettings::ToggleQuality(KPstateContext *pContext)
{
    auto &config = pContext->GetConfig();
    auto E_FullScreen_before = E_FullScreen;
    auto E_WindowWidth_before = E_WindowWidth;

    pContext->GetUserInterface().PlayAudio(KP_SND_CHANGESETTING);
    UpdateQuality(pContext);
    Quality--;

    if (Quality <= 0)
    {
        Quality = 5;
    }

    switch (Quality)
    {
        case 1:
        {
            E_FullScreen = false;
            E_WindowWidth = 640;
            config.TextureSize = 8;
            config.MenuTextureSize = 2;
            config.Nearest = true;
            config.LightSources = 2;
        }
        break;

        case 2:
        {
            E_FullScreen = false;
            E_WindowWidth = 800;
            config.TextureSize = 4;
            config.MenuTextureSize = 2;
            config.Nearest = true;
            config.LightSources = 2;
        }
        break;

        case 3:
        {
            E_FullScreen = false;
            E_WindowWidth = 1024;
            config.TextureSize = 2;
            config.MenuTextureSize = 1;
            config.Nearest = true;
            config.LightSources = 2;
        }
        break;

        case 4:
        {
            E_FullScreen = false;
            E_WindowWidth = 1280;
            config.TextureSize = 1;
            config.MenuTextureSize = 1;
            config.Nearest = false;
            config.LightSources = 3;
        }
        break;

        default:
        {
            E_FullScreen = pContext->GetUserInterface().CanToggleFullScreen();
            E_WindowWidth = 1600;
            config.TextureSize = 1;
            config.MenuTextureSize = 1;
            config.Nearest = false;
            config.LightSources = 3;
        }
        break;
    }

    pContext->GetLight().Update(config.AmbientLight,
                                config.LightSources);
    pContext->GetMenu().Update(config.TextureName,
                               config.MenuTextureSize,
                               config.Nearest);
    pContext->GetBoardView().InitializeTextures(
        config.GetDirectory(KP_TEXTURE_DIR),
        config.TextureName,
        config.TextureSize,
        config.Nearest);

    if (E_FullScreen_before != E_FullScreen &&
        (pContext->GetUserInterface().CanToggleFullScreen()))
    {
        pContext->GetUserInterface().SetWindowMode(E_FullScreen);
    }

    if (!E_FullScreen &&
        (E_WindowWidth_before != E_WindowWidth) &&
        (pContext->GetUserInterface().CanChangeWindowSize()))
    {
        pContext->GetUserInterface().SetWindowSize(E_WindowWidth,
                (E_WindowWidth * 3) / 4);
    }

    UpdateDisplay(pContext);
}

int KPstateGraphicSettings::GetTextureIndex(KPstateContext *pContext,
        std::string &TextureName) const
{
    auto textureNameList = pContext->GetConfig().GetTextureNames();
    auto index = 0;

    std::sort(textureNameList.begin(), textureNameList.end());
    auto it = textureNameList.cbegin();

    while (it != textureNameList.cend() && *it != TextureName)
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

int KPstateGraphicSettings::GetWindowWidthsIndex(int width) const
{
    auto maxSize = sizeof(windowWidths) / sizeof(windowWidths[0]);
    decltype(maxSize) index = 0;

    for (index = 0; index < maxSize; ++index)
    {
        if (windowWidths[index] == width)
        {
            return index;
        }
    }

    return -1; // index not found
}
