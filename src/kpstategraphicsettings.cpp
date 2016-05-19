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
                                        StateId previousStateId)
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
        auto labelId = GetLabelId(Lbl::TextureName1, index);

        if (pContext->GetMenu().labels.find(labelId) ==
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
        menu.labels[Lbl::Fps].SetPosition(0, 11.7f, 0.3f);
    }

    menu.plates[KPPlate::MenuBackground].SetPosition(2, 1.7f, 14, 9.5f);

    menu.plates[KPPlate::Logo].SetPosition(4, 10, 12, 12);
    menu.plates[KPPlate::Logo].SetFullyVisible();

    menu.labels[Lbl::Settings].SetPosition(4, 9, 1);
    menu.labels[Lbl::Settings].SetFullyVisible();
    menu.labels[Lbl::Graphics].SetPosition(12, 9, 1, AlignItem::Right);
    menu.labels[Lbl::Graphics].SetFullyVisible();

    auto y = 8.0f;
    menu.labels[Lbl::Quality].SetPosition(8, y, 1, AlignItem::Right);
    menu.labels[Lbl::Quality].SetSignal(Signal::ToggleQuality);
    y = 7.4f;

    if (E_FullScreen == 0)
    {
        menu.labels[Lbl::Resolution].SetPosition(8, y, 0.6f, AlignItem::Right);
        menu.labels[Lbl::Resolution].SetSignal(Signal::ToggleResolution);
    }

    y -= dy;

    if (pContext->GetUserInterface().CanToggleFullScreen())
    {
        menu.labels[Lbl::Screenmode].SetPosition(8, y, 0.6f, AlignItem::Right);
        menu.labels[Lbl::Screenmode].SetSignal(Signal::ToggleScreenmode);
        y -= dy;
    }

    menu.labels[Lbl::MenuTextures].SetPosition(8, y, 0.6f, AlignItem::Right);
    y -= dy;
    menu.labels[Lbl::MenuTextures].SetSignal(Signal::ToggleMenuTextures);
    menu.labels[Lbl::Textures].SetPosition(8, y, 0.6f, AlignItem::Right);
    y -= dy;
    menu.labels[Lbl::Textures].SetSignal(Signal::ToggleTextures);
    menu.labels[Lbl::TextureName].SetPosition(8, y, 0.6f, AlignItem::Right);
    y -= dy;
    menu.labels[Lbl::TextureName].SetSignal(Signal::ToggleTextureName);
    menu.labels[Lbl::TextureInterpolation].SetPosition(8, y, 0.6f,
            AlignItem::Right);
    y -= dy;
    menu.labels[Lbl::TextureInterpolation].SetSignal(
        Signal::ToggleTextureInterpolation);
    menu.labels[Lbl::AmbientLight].SetPosition(8, y, 0.6f, AlignItem::Right);
    y -= dy;
    menu.labels[Lbl::AmbientLight].SetSignal(Signal::ToggleAmbientLight);
    menu.labels[Lbl::Illumination].SetPosition(8, y, 0.6f, AlignItem::Right);
    y -= dy;
    menu.labels[Lbl::Illumination].SetSignal(Signal::ToggleIllumination);
#if defined(HAVE_LIBGLUT) || defined(HAVE_LIBOPENGLUT)
    menu.labels[Lbl::UserInterface].SetPosition(8, y, 0.6f, AlignItem::Right);
    y -= dy;
    menu.labels[Lbl::UserInterface].SetSignal(Signal::ToggleUserInterface);
#endif
    menu.labels[Lbl::Framerate].SetPosition(8, y, 0.6f, AlignItem::Right);
    y -= dy;
    menu.labels[Lbl::Framerate].SetSignal(Signal::ToggleFramerate);

    y = 8.0;

    switch (Quality)
    {
        case 1:
        {
            menu.labels[Lbl::QualityVeryFast].SetPosition(8.2f, y, 1);
            menu.labels[Lbl::QualityVeryFast].SetSignal(Signal::ToggleQuality);
        }
        break;

        case 2:
        {
            menu.labels[Lbl::QualityFast].SetPosition(8.2f, y, 1);
            menu.labels[Lbl::QualityFast].SetSignal(Signal::ToggleQuality);
        }
        break;

        case 3:
        {
            menu.labels[Lbl::QualityNormal].SetPosition(8.2f, y, 1);
            menu.labels[Lbl::QualityNormal].SetSignal(Signal::ToggleQuality);
        }
        break;

        case 4:
        {
            menu.labels[Lbl::QualityHigh].SetPosition(8.2f, y, 1);
            menu.labels[Lbl::QualityHigh].SetSignal(Signal::ToggleQuality);
        }
        break;

        case 5:
        {
            menu.labels[Lbl::QualityVeryHigh].SetPosition(8.2f, y, 1);
            menu.labels[Lbl::QualityVeryHigh].SetSignal(Signal::ToggleQuality);
        }
        break;

        default:
        {
            menu.labels[Lbl::QualityUserDefined].SetPosition(8.2f, y, 1);
            menu.labels[Lbl::QualityUserDefined].SetSignal(
                Signal::ToggleQuality);
        }
        break;
    }

    y = 7.4f;

    if (!E_FullScreen)
    {
        static const Lbl windowResolutionLabels[] =
        {
            Lbl::Resolution640x480, Lbl::Resolution800x600,
            Lbl::Resolution1024x768, Lbl::Resolution1280x960,
            Lbl::Resolution1600x1200, Lbl::Resolution1920x1440
        };

        auto index = GetWindowWidthsIndex(E_WindowWidth);

        if (index >= 0)
        {
            auto windowResolutionLabel = windowResolutionLabels[index];

            menu.labels[windowResolutionLabel].SetPosition(8.2f, y, 0.6f);
            menu.labels[windowResolutionLabel].SetSignal(
                Signal::ToggleResolution);
        }
    }

    if (pContext->GetUserInterface().CanToggleFullScreen())
    {
        y -= dy;

        if (E_FullScreen)
        {
            menu.labels[Lbl::ScreenModeFullscreen].SetPosition(8.2f, y, 0.6f);
            menu.labels[Lbl::ScreenModeFullscreen].SetSignal(
                Signal::ToggleScreenmode);
        }
        else
        {
            menu.labels[Lbl::ScreenModeWindow].SetPosition(8.2f, y, 0.6f);
            menu.labels[Lbl::ScreenModeWindow].SetSignal(
                Signal::ToggleScreenmode);
        }
    }

    y -= dy;

    switch (config.MenuTextureSize)
    {
        case 1:
        {
            menu.labels[Lbl::MenuTexturesHigh].SetPosition(8.2f, y, 0.6f);
            menu.labels[Lbl::MenuTexturesHigh].SetSignal(
                Signal::ToggleMenuTextures);
        }
        break;

        case 2:
        {
            menu.labels[Lbl::MenuTexturesNormal].SetPosition(8.2f, y, 0.6f);
            menu.labels[Lbl::MenuTexturesNormal].SetSignal(
                Signal::ToggleMenuTextures);
        }
        break;
    }

    y -= dy;

    switch (config.TextureSize)
    {
        case 1:
        {
            menu.labels[Lbl::TexturesHigh].SetPosition(8.2f, y, 0.6f);
            menu.labels[Lbl::TexturesHigh].SetSignal(Signal::ToggleTextures);
        }
        break;

        case 2:
        {
            menu.labels[Lbl::TexturesNormal].SetPosition(8.2f, y, 0.6f);
            menu.labels[Lbl::TexturesNormal].SetSignal(Signal::ToggleTextures);
        }
        break;

        case 4:
        {
            menu.labels[Lbl::TexturesLow].SetPosition(8.2f, y, 0.6f);
            menu.labels[Lbl::TexturesLow].SetSignal(Signal::ToggleTextures);
        }
        break;

        case 8:
        {
            menu.labels[Lbl::TexturesVeryLow].SetPosition(8.2f, y, 0.6f);
            menu.labels[Lbl::TexturesVeryLow].SetSignal(Signal::ToggleTextures);
        }
        break;
    }

    y -= dy;
    menu.labels[GetLabelId(Lbl::TextureName1, textureIndex)].SetPosition(
        8.2f, y, 0.6f);
    menu.labels[GetLabelId(Lbl::TextureName1, textureIndex)].SetSignal(
        Signal::ToggleTextureName);

    y -= dy;

    if (config.Nearest)
    {
        menu.labels[Lbl::TextureInterpolationOff].SetPosition(8.2f, y, 0.6f);
        menu.labels[Lbl::TextureInterpolationOff].SetSignal(
            Signal::ToggleTextureInterpolation);
    }
    else
    {
        menu.labels[Lbl::TextureInterpolationOn].SetPosition(8.2f, y, 0.6f);
        menu.labels[Lbl::TextureInterpolationOn].SetSignal(
            Signal::ToggleTextureInterpolation);
    }

    y -= dy;

    if (config.AmbientLight)
    {
        menu.labels[Lbl::AmbientLightOn].SetPosition(8.2f, y, 0.6f);
        menu.labels[Lbl::AmbientLightOn].SetSignal(Signal::ToggleAmbientLight);
    }
    else
    {
        menu.labels[Lbl::AmbientLightOff].SetPosition(8.2f, y, 0.6f);
        menu.labels[Lbl::AmbientLightOff].SetSignal(Signal::ToggleAmbientLight);

    }

    y -= dy;

    switch (config.LightSources)
    {
        case 1:
        {
            menu.labels[Lbl::Illumination1].SetPosition(8.2f, y, 0.6f);
            menu.labels[Lbl::Illumination1].SetSignal(
                Signal::ToggleIllumination);
        }
        break;

        case 2:
        {
            menu.labels[Lbl::Illumination2].SetPosition(8.2f, y, 0.6f);
            menu.labels[Lbl::Illumination2].SetSignal(
                Signal::ToggleIllumination);
        }
        break;

        case 3:
        {
            menu.labels[Lbl::Illumination3].SetPosition(8.2f, y, 0.6f);
            menu.labels[Lbl::Illumination3].SetSignal(
                Signal::ToggleIllumination);
        }
        break;
    }

#if defined(HAVE_LIBGLUT) || defined(HAVE_LIBOPENGLUT)
    y -= dy;

    switch (E_UserInterface)
    {
        case 0:
        {
            menu.labels[Lbl::UserInterface0].SetPosition(8.2f, y, 0.6f);
            menu.labels[Lbl::UserInterface0].SetSignal(
                Signal::ToggleUserInterface);
        }
        break;

        case 1:
        {
            menu.labels[Lbl::UserInterface1].SetPosition(8.2f, y, 0.6f);
            menu.labels[Lbl::UserInterface1].SetSignal(
                Signal::ToggleUserInterface);
        }
        break;
    }

#endif

    y -= dy;

    if (config.DisplayFPS)
    {
        menu.labels[Lbl::FramerateOn].SetPosition(8.2f, y, 0.6f);
        menu.labels[Lbl::FramerateOn].SetSignal(Signal::ToggleFramerate);
    }
    else
    {
        menu.labels[Lbl::FramerateOff].SetPosition(8.2f, y, 0.6f);
        menu.labels[Lbl::FramerateOff].SetSignal(Signal::ToggleFramerate);
    }

    menu.labels[Lbl::Back].SetPosition(8, 0.7f, 1 , AlignItem::Centered);
    menu.labels[Lbl::Back].SetSignal(Signal::Back);

    StartAnimation(pContext);
}

void KPstateGraphicSettings::MouseClick(KPstateContext *pContext,
                                        MouseButton button,
                                        MouseButtonEvent event,
                                        int x, int y)
{
    auto signal = KPstate::EvaluateMouseClick(pContext, button, event, x, y);

    switch (signal)
    {
        case Signal::ToggleFramerate:
            ToggleFPS(pContext);
            UpdateDisplay(pContext);
            break;

        case Signal::ToggleIllumination:
            ToggleLamps(pContext);
            UpdateDisplay(pContext);
            break;

        case Signal::ToggleScreenmode:
            ToggleScreenMode(pContext);
            UpdateDisplay(pContext);
            break;

        case Signal::ToggleResolution:
            ToggleResolution(pContext);
            UpdateDisplay(pContext);
            break;

        case Signal::ToggleTextureInterpolation:
            ToggleTextureInterpolation(pContext);
            UpdateDisplay(pContext);
            break;

        case Signal::ToggleTextures:
            ToggleTextures(pContext);
            UpdateDisplay(pContext);
            break;

        case Signal::ToggleMenuTextures:
            ToggleMenuTextures(pContext);
            UpdateDisplay(pContext);
            break;

        case Signal::ToggleTextureName:
            ToggleTextureName(pContext);
            UpdateDisplay(pContext);
            break;

        case Signal::ToggleAmbientLight:
            ToggleAmbientLight(pContext);
            UpdateDisplay(pContext);
            break;

#if defined(HAVE_LIBGLUT) || defined(HAVE_LIBOPENGLUT)

        case Signal::ToggleUserInterface:
            ToggleUserInterface(pContext);
            UpdateDisplay(pContext);
            break;
#endif

        case Signal::ToggleQuality:
            ToggleQuality(pContext);
            UpdateDisplay(pContext);
            break;

        case Signal::Back:
        {
            auto newState = SaveChanges(pContext);
            pContext->ChangeState(newState);
            return;
        }

        default:
            break;
    }
}

StateId KPstateGraphicSettings::ESCKeyAction(
    KPstateContext *pContext) const
{
    auto newState = SaveChanges(pContext);
    return newState;
}

/////////////////////////////////////////////////////////////////////
// Take over changes and change state
/////////////////////////////////////////////////////////////////////

StateId KPstateGraphicSettings::SaveChanges(KPstateContext *pContext) const
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
        return StateId::GraphicHint;
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
    pContext->GetUserInterface().PlayAudio(KPSound::ChangeSetting);
    pContext->GetConfig().DisplayFPS = !pContext->GetConfig().DisplayFPS;
}

void KPstateGraphicSettings::ToggleAmbientLight(KPstateContext *pContext) const
{
    auto &config = pContext->GetConfig();

    pContext->GetUserInterface().PlayAudio(KPSound::ChangeSetting);
    pContext->GetConfig().AmbientLight = !config.AmbientLight;
    pContext->GetLight().Update(config.AmbientLight,
                                config.LightSources);
}

void KPstateGraphicSettings::ToggleLamps(KPstateContext *pContext)
{
    auto &config = pContext->GetConfig();

    pContext->GetUserInterface().PlayAudio(KPSound::ChangeSetting);

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

    pContext->GetUserInterface().PlayAudio(KPSound::ChangeSetting);

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
        config.Get(KPDirectory::Texture),
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
            config.Get(KPDirectory::Texture),
            config.TextureName,
            config.TextureSize,
            config.Nearest,
            false);

        pContext->GetMenu().Update(config.TextureName,
                                   config.MenuTextureSize,
                                   config.Nearest);
        pContext->GetUserInterface().InitializeAudio(config.TextureName);

        pContext->GetUserInterface().PlayAudio(KPSound::ChangeSetting);
    }
}

void KPstateGraphicSettings::ToggleScreenMode(KPstateContext *pContext)
{
    E_FullScreen = !E_FullScreen;
    pContext->GetUserInterface().PlayAudio(KPSound::ChangeSetting);

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

    userIfc.PlayAudio(KPSound::ChangeSetting);

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
    pContext->GetUserInterface().PlayAudio(KPSound::ChangeSetting);

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

    pContext->GetUserInterface().PlayAudio(KPSound::ChangeSetting);

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

    pContext->GetUserInterface().PlayAudio(KPSound::ChangeSetting);
    config.Nearest = !config.Nearest;
    pContext->GetMenu().Update(config.TextureName,
                               config.MenuTextureSize,
                               config.Nearest);
    pContext->GetBoardView().InitializeTextures(
        config.Get(KPDirectory::Texture),
        config.TextureName,
        config.TextureSize,
        config.Nearest);
}

void KPstateGraphicSettings::ToggleQuality(KPstateContext *pContext)
{
    auto &config = pContext->GetConfig();
    auto E_FullScreen_before = E_FullScreen;
    auto E_WindowWidth_before = E_WindowWidth;

    pContext->GetUserInterface().PlayAudio(KPSound::ChangeSetting);
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
        config.Get(KPDirectory::Texture),
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
