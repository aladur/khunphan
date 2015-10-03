/*
    kpstategraphicsettings.cpp

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

#include "kpstategraphicsettings.h"
#include <algorithm>
#include "kpmenu.h"
#include "kpboardGL.h"
#include "kpconfig.h"
#include "light.h"
#include "kpuibase.h"


void KPstateGraphicSettings::Initialize(KPstateContext *pContext,
                                        const KPstate *pOldState)
{
    KPstate::Initialize(pContext, pOldState);

    E_ScreenXResolution = KPConfig::Instance().ScreenXResolution;
    E_ColorDepth        = KPConfig::Instance().ColorDepth;
    E_UserInterface     = KPConfig::Instance().UserInterface;
    E_FullScreen        = KPConfig::Instance().FullScreen;
    Quality             = 0;

    textureIndex        = 0;
    textureNameList     = KPConfig::Instance().GetTextureNames();
    if (textureNameList.empty())
    {
        message(mtError, "*** Error: No texture directories found\n");
        exit(1);
    }
    std::sort(textureNameList.begin(), textureNameList.end());
    for (unsigned int i = 0; i < textureNameList.size(); i++)
    {
        if (pContext->GetMenu().labels.find(T_TEXTURE1 + i) ==
            pContext->GetMenu().labels.end())
        {
            message(mtError, "*** Error: Not enough Language entries for %d "
                    "Textures\n",
                    textureNameList.size());
            exit(1);
        }
    }

    textureIndex = GetTextureIndex(KPConfig::Instance().TextureName);

    UpdateDisplay(pContext);
}

void KPstateGraphicSettings::UpdateDisplay(KPstateContext *pContext)
{
    KPstate::UpdateDisplay(pContext);

    KPmenu &menu = pContext->GetMenu();

    float y;
    float dy = 0.5;

    if (KPConfig::Instance().DisplayFPS)
    {
        menu.labels[T_FPS].SetPosition(0, 11.7f, 0.3f);
    }

    if (E_FullScreen==false &&
        E_ScreenXResolution==640 &&
        E_ColorDepth==16 &&
        KPConfig::Instance().TextureSize==8 &&
        KPConfig::Instance().MenuTextureSize==2 &&
        KPConfig::Instance().Reflections==false &&
        KPConfig::Instance().Shadows==false &&
        KPConfig::Instance().Nearest==true &&
        KPConfig::Instance().LightSources==2)
    {
        Quality=1;
    }
    else if (E_FullScreen==false &&
             E_ScreenXResolution==640 &&
             E_ColorDepth==16 &&
             KPConfig::Instance().TextureSize==4 &&
             KPConfig::Instance().MenuTextureSize==2 &&
             KPConfig::Instance().Reflections==false &&
             KPConfig::Instance().Shadows==true &&
             KPConfig::Instance().Nearest==true &&
             KPConfig::Instance().LightSources==2)
    {
        Quality=2;
    }
    else if (E_FullScreen==false &&
             E_ScreenXResolution==800 &&
             E_ColorDepth==16 &&
             KPConfig::Instance().TextureSize==2 &&
             KPConfig::Instance().MenuTextureSize==1 &&
             KPConfig::Instance().Reflections==false &&
             KPConfig::Instance().Shadows==true &&
             KPConfig::Instance().Nearest==true &&
             KPConfig::Instance().LightSources==2)
    {
        Quality=3;
    }
    else if (E_FullScreen==false &&
             E_ScreenXResolution==1024 &&
             E_ColorDepth==16 &&
             KPConfig::Instance().TextureSize==1 &&
             KPConfig::Instance().MenuTextureSize==1 &&
             KPConfig::Instance().Reflections==false &&
             KPConfig::Instance().Shadows==true &&
             KPConfig::Instance().Nearest==false &&
             KPConfig::Instance().LightSources==3)
    {
        Quality=4;
    }
    else if (E_FullScreen==true &&
             E_ScreenXResolution==1024 &&
             E_ColorDepth==32 &&
             KPConfig::Instance().TextureSize==1 &&
             KPConfig::Instance().MenuTextureSize==1 &&
             KPConfig::Instance().Reflections==true &&
             KPConfig::Instance().Shadows==true &&
             KPConfig::Instance().Nearest==false &&
             KPConfig::Instance().LightSources==3)
    {
        Quality=5;
    }
    else
    {
        Quality=0;
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
    menu.labels[T_SCREENMODE].SetPosition(8, y, 0.6f, A_RIGHT);
    y -= dy;
    menu.labels[T_SCREENMODE].SetSignal(S_TOGGLE_SCREENMODE);
    menu.labels[T_MENUTEXTURES].SetPosition(8, y, 0.6f, A_RIGHT);
    y -= dy;
    menu.labels[T_MENUTEXTURES].SetSignal(S_TOGGLE_MENUTEXTURES);
    menu.labels[T_TEXTURES].SetPosition(8, y, 0.6f, A_RIGHT);
    y -= dy;
    menu.labels[T_TEXTURES].SetSignal(S_TOGGLE_TEXTURES);
    menu.labels[T_TEXTURENAME].SetPosition(8, y, 0.6f, A_RIGHT);
    y -= dy;
    menu.labels[T_TEXTURENAME].SetSignal(S_TOGGLE_TEXTURENAME);
    menu.labels[T_REFLECTIONS].SetPosition(8, y, 0.6f, A_RIGHT);
    y -= dy;
    menu.labels[T_REFLECTIONS].SetSignal(S_TOGGLE_REFLECTIONS);
    menu.labels[T_COLORDEPTH].SetPosition(8, y, 0.6f, A_RIGHT);
    y -= dy;
    menu.labels[T_COLORDEPTH].SetSignal(S_TOGGLE_COLORDEPTH);
    // Shadows currently not supported
    //menu.labels[T_SHADOWS].SetPosition(8,y,0.6f,A_RIGHT);
    //y -= dy;
    //menu.labels[T_SHADOWS].SetSignal(S_TOGGLE_SHADOWS);
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

    y -= dy;
    switch (KPConfig::Instance().MenuTextureSize)
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
    switch (KPConfig::Instance().TextureSize)
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
    if (KPConfig::Instance().Reflections)
    {
        menu.labels[T_REF_ON].SetPosition(8.2f, y, 0.6f);
        menu.labels[T_REF_ON].SetSignal(S_TOGGLE_REFLECTIONS);
    }
    else
    {
        menu.labels[T_REF_OFF].SetPosition(8.2f, y, 0.6f);
        menu.labels[T_REF_OFF].SetSignal(S_TOGGLE_REFLECTIONS);
    }

    y -= dy;
    switch (E_ColorDepth)
    {
        case 32:
        {
            menu.labels[T_CD_32].SetPosition(8.2f, y, 0.6f);
            menu.labels[T_CD_32].SetSignal(S_TOGGLE_COLORDEPTH);
        }
        break;
        default:
        {
            menu.labels[T_CD_16].SetPosition(8.2f, y, 0.6f);
            menu.labels[T_CD_16].SetSignal(S_TOGGLE_COLORDEPTH);
        }
        break;
    }

    /*
    y -= dy;
    if (KPConfig::Instance().Shadows)
    {
      menu.labels[T_S_ON].SetPosition(8.2f, y, 0.6f);
      menu.labels[T_S_ON].SetSignal(S_TOGGLE_SHADOWS);
    }
    else
    {
      menu.labels[T_S_OFF].SetPosition(8.2f, y, 0.6f);
      menu.labels[T_S_OFF].SetSignal(S_TOGGLE_SHADOWS);
    }
    */

    y -= dy;
    if (KPConfig::Instance().Nearest)
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
    if (KPConfig::Instance().AmbientLight)
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
    switch (KPConfig::Instance().LightSources)
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
    if (KPConfig::Instance().DisplayFPS)
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

    StartAnimation();
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
        case S_TOGGLE_REFLECTIONS:
            ToggleReflections(pContext);
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
        case S_TOGGLE_COLORDEPTH:
            ToggleColorDepth(pContext);
            UpdateDisplay(pContext);
            break;
        case S_TOGGLE_SHADOWS:
            ToggleShadows(pContext);
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

void KPstateGraphicSettings::KeyPressed (KPstateContext *pContext,
        unsigned char key, int x, int y)
{
    CHECK_DEFAULT_KEY_PRESSED(pContext, key, x, y);
}

tKPMenuState KPstateGraphicSettings::ESCKeyAction (KPstateContext *pContext)
{
    tKPMenuState newState;

    newState = SaveChanges(pContext);
    return newState;
}

/////////////////////////////////////////////////////////////////////
// Take over changes and change state
/////////////////////////////////////////////////////////////////////

tKPMenuState KPstateGraphicSettings::SaveChanges(KPstateContext *)
{
    bool ResolutionChanged = false;

    if (KPConfig::Instance().ColorDepth        != E_ColorDepth         ||
        KPConfig::Instance().FullScreen        != E_FullScreen         ||
        KPConfig::Instance().ScreenXResolution != E_ScreenXResolution  ||
        KPConfig::Instance().UserInterface     != E_UserInterface)
    {
        ResolutionChanged = true;
    }

    LOG1("Changed Graphic Settings");
    LOG2("  UserInterface :        ",
         KPConfig::GetUserInterfaceName(E_UserInterface));
    LOG2("  TextureName :          ", KPConfig::Instance().TextureName);
    LOG2("  TextureSize :          ", KPConfig::Instance().TextureSize);
    LOG2("  MenuTextureSize :      ", KPConfig::Instance().MenuTextureSize);
    LOG2("  Shadows :              ", ON_OFF(KPConfig::Instance().Shadows));
    LOG2("  TextureInterpolation : ", ON_OFF(KPConfig::Instance().Nearest));
    LOG2("  ScreenXResolution :    ", E_ScreenXResolution);
    LOG2("  ColorDepth :           ", E_ColorDepth);
    LOG2("  Reflections :          ", ON_OFF(KPConfig::Instance().Reflections));
    LOG2("  AmbientLight :         ",
         ON_OFF(KPConfig::Instance().AmbientLight));
    LOG2("  LightSources :         ", KPConfig::Instance().LightSources);
    LOG2("  FullScreen :           ", ON_OFF(E_FullScreen));

    KPConfig::Instance().ColorDepth        = E_ColorDepth;
    KPConfig::Instance().FullScreen        = E_FullScreen;
    KPConfig::Instance().ScreenXResolution = E_ScreenXResolution;
    KPConfig::Instance().UserInterface     = E_UserInterface;

    KPConfig::Instance().WriteToFile();

    if (ResolutionChanged)
    {
        return KPState_GraphicHint;
    }
    else
    {
        return oldStateId;
    }
}

/////////////////////////////////////////////////////////////////////
// Toggle settings
/////////////////////////////////////////////////////////////////////

void KPstateGraphicSettings::ToggleFPS(KPstateContext *pContext)
{
    pContext->GetUserInterface().PlayAudio(KP_SND_CHANGESETTING);
    KPConfig::Instance().DisplayFPS = !KPConfig::Instance().DisplayFPS;
}

void KPstateGraphicSettings::ToggleAmbientLight(KPstateContext *pContext)
{
    pContext->GetUserInterface().PlayAudio(KP_SND_CHANGESETTING);
    KPConfig::Instance().AmbientLight = !KPConfig::Instance().AmbientLight;
    pContext->GetLight().Update(KPConfig::Instance().AmbientLight,
                                KPConfig::Instance().LightSources,
                                KPConfig::Instance().Reflections);
}

void KPstateGraphicSettings::ToggleReflections(KPstateContext *pContext)
{
    pContext->GetUserInterface().PlayAudio(KP_SND_CHANGESETTING);
    KPConfig::Instance().Reflections = !KPConfig::Instance().Reflections;
    pContext->GetLight().Update(KPConfig::Instance().AmbientLight,
                                KPConfig::Instance().LightSources,
                                KPConfig::Instance().Reflections);
}

void KPstateGraphicSettings::ToggleLamps(KPstateContext *pContext)
{
    pContext->GetUserInterface().PlayAudio(KP_SND_CHANGESETTING);
    switch (KPConfig::Instance().LightSources)
    {
        case 1:
            KPConfig::Instance().LightSources = 2;
            break;
        case 2:
            KPConfig::Instance().LightSources = 3;
            break;
        case 3:
            KPConfig::Instance().LightSources = 1;
            break;
        default:
            KPConfig::Instance().LightSources = 1;
            break;
    }
    pContext->GetLight().Update(KPConfig::Instance().AmbientLight,
                                KPConfig::Instance().LightSources,
                                KPConfig::Instance().Reflections);
}

void KPstateGraphicSettings::ToggleTextures(KPstateContext *pContext)
{
    pContext->GetUserInterface().PlayAudio(KP_SND_CHANGESETTING);
    switch (KPConfig::Instance().TextureSize)
    {
        case 1:
            KPConfig::Instance().TextureSize  = 2;
            break;
        case 2:
            KPConfig::Instance().TextureSize  = 4;
            break;
        case 4:
            KPConfig::Instance().TextureSize  = 8;
            break;
        case 8:
            KPConfig::Instance().TextureSize  = 1;
            break;
        default:
            KPConfig::Instance().TextureSize = 1;
            break;
    }
    pContext->GetBoardView().InitializeTextures(
        KPConfig::Instance().TextureName.c_str(),
        KPConfig::Instance().TextureSize,
        KPConfig::Instance().Nearest );
}

void KPstateGraphicSettings::ToggleTextureName(KPstateContext *pContext)
{
    if (textureNameList.size() > 1)
    {
        std::vector<std::string>::const_iterator it;

        it = std::find(textureNameList.begin(), textureNameList.end(),
                       KPConfig::Instance().TextureName);
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
        KPConfig::Instance().TextureName = *it;
        textureIndex = GetTextureIndex(KPConfig::Instance().TextureName);
        pContext->GetBoardView().InitializeTextures(
            KPConfig::Instance().TextureName.c_str(),
            KPConfig::Instance().TextureSize,
            KPConfig::Instance().Nearest,
            false );

        pContext->GetMenu().Update( KPConfig::Instance().TextureName,
                                    KPConfig::Instance().MenuTextureSize,
                                    KPConfig::Instance().Nearest,
                                    false );
        pContext->GetUserInterface().InitializeAudio(
            KPConfig::Instance().TextureName.c_str(), true);

        pContext->GetUserInterface().PlayAudio(KP_SND_CHANGESETTING);
    }
}

void KPstateGraphicSettings::ToggleScreenMode(KPstateContext *pContext)
{
    pContext->GetUserInterface().PlayAudio(KP_SND_CHANGESETTING);
    E_FullScreen = !E_FullScreen;
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
}

void KPstateGraphicSettings::ToggleColorDepth(KPstateContext *pContext)
{
    pContext->GetUserInterface().PlayAudio(KP_SND_CHANGESETTING);
    switch (E_ColorDepth)
    {
        case 16:
            E_ColorDepth = 32;
            break;
        default:
            E_ColorDepth = 16;
            break;
    }
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

void KPstateGraphicSettings::ToggleShadows(KPstateContext *pContext)
{
    pContext->GetUserInterface().PlayAudio(KP_SND_CHANGESETTING);
    KPConfig::Instance().Shadows = !KPConfig::Instance().Shadows;
}

void KPstateGraphicSettings::ToggleMenuTextures(KPstateContext *pContext)
{
    pContext->GetUserInterface().PlayAudio(KP_SND_CHANGESETTING);
    switch (KPConfig::Instance().MenuTextureSize)
    {
        case 1:
            KPConfig::Instance().MenuTextureSize = 2;
            break;
        case 2:
            KPConfig::Instance().MenuTextureSize = 1;
            break;
        default:
            KPConfig::Instance().MenuTextureSize = 1;
            break;
    }
    pContext->GetMenu().Update( KPConfig::Instance().TextureName,
                                KPConfig::Instance().MenuTextureSize,
                                KPConfig::Instance().Nearest );
}

void KPstateGraphicSettings::ToggleTextureInterpolation(
    KPstateContext *pContext)
{
    pContext->GetUserInterface().PlayAudio(KP_SND_CHANGESETTING);
    KPConfig::Instance().Nearest = !KPConfig::Instance().Nearest;
    pContext->GetMenu().Update( KPConfig::Instance().TextureName,
                                KPConfig::Instance().MenuTextureSize,
                                KPConfig::Instance().Nearest );
    pContext->GetBoardView().InitializeTextures(
        KPConfig::Instance().TextureName.c_str(),
        KPConfig::Instance().TextureSize,
        KPConfig::Instance().Nearest );
}

void KPstateGraphicSettings::ToggleQuality(KPstateContext *pContext)
{
    pContext->GetUserInterface().PlayAudio(KP_SND_CHANGESETTING);
    switch(Quality)
    {
        case 1:
        {
            E_FullScreen=true;
            E_ScreenXResolution=1024;
            E_ColorDepth=32;
            KPConfig::Instance().TextureSize=1;
            KPConfig::Instance().MenuTextureSize=1;
            KPConfig::Instance().Reflections=true;
            KPConfig::Instance().Shadows=true;
            KPConfig::Instance().Nearest=false;
            KPConfig::Instance().LightSources=3;
        }
        break;
        case 2:
        {
            E_FullScreen=false;
            E_ScreenXResolution=640;
            E_ColorDepth=16;
            KPConfig::Instance().TextureSize=8;
            KPConfig::Instance().MenuTextureSize=2;
            KPConfig::Instance().Reflections=false;
            KPConfig::Instance().Shadows=false;
            KPConfig::Instance().Nearest=true;
            KPConfig::Instance().LightSources=2;
        }
        break;
        case 3:
        {
            E_FullScreen=false;
            E_ScreenXResolution=640;
            E_ColorDepth=16;
            KPConfig::Instance().TextureSize=4;
            KPConfig::Instance().MenuTextureSize=2;
            KPConfig::Instance().Reflections=false;
            KPConfig::Instance().Shadows=true;
            KPConfig::Instance().Nearest=true;
            KPConfig::Instance().LightSources=2;
        }
        break;
        case 4:
        {
            E_FullScreen=false;
            E_ScreenXResolution=800;
            E_ColorDepth=16;
            KPConfig::Instance().TextureSize=2;
            KPConfig::Instance().MenuTextureSize=1;
            KPConfig::Instance().Reflections=false;
            KPConfig::Instance().Shadows=true;
            KPConfig::Instance().Nearest=true;
            KPConfig::Instance().LightSources=2;
        }
        break;
        case 5:
        {
            E_FullScreen=false;
            E_ScreenXResolution=1024;
            E_ColorDepth=16;
            KPConfig::Instance().TextureSize=1;
            KPConfig::Instance().MenuTextureSize=1;
            KPConfig::Instance().Reflections=false;
            KPConfig::Instance().Shadows=true;
            KPConfig::Instance().Nearest=false;
            KPConfig::Instance().LightSources=3;
        }
        break;
        default:
        {
            E_FullScreen=false;
            E_ScreenXResolution=800;
            E_ColorDepth=16;
            KPConfig::Instance().TextureSize=2;
            KPConfig::Instance().MenuTextureSize=1;
            KPConfig::Instance().Reflections=false;
            KPConfig::Instance().Shadows=true;
            KPConfig::Instance().Nearest=true;
            KPConfig::Instance().LightSources=2;
        }
        break;
    }
    pContext->GetLight().Update(KPConfig::Instance().AmbientLight,
                                KPConfig::Instance().LightSources,
                                KPConfig::Instance().Reflections);
    pContext->GetMenu().Update( KPConfig::Instance().TextureName,
                                KPConfig::Instance().MenuTextureSize,
                                KPConfig::Instance().Nearest );
    pContext->GetBoardView().InitializeTextures(
        KPConfig::Instance().TextureName.c_str(),
        KPConfig::Instance().TextureSize,
        KPConfig::Instance().Nearest );
}

int KPstateGraphicSettings::GetTextureIndex(std::string &TextureName)
{
    int index = 0;

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
