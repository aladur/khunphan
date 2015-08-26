/*
    kpstategraphicsettings.h

    Automatic solution finder for KhunPhan game
    Copyright (C) 2001,2002,2003  W. Schwotzer

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


#ifdef WIN32
#ifndef WARN_TRUNC_DOUBLE_FLOAT
#pragma warning (disable:4305)  /* Disable warning to truncate from double to float */
#endif
#endif


void KPstateGraphicSettings::Initialize(KPstateContext *pContext,  const KPstate *pOldState)
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
    if (pContext->GetMenu().TextfeldArray.find(T_TEXTURE1 + i) ==
        pContext->GetMenu().TextfeldArray.end())
    {
      message(mtError, "*** Error: Not enough Language entries for %d Textures\n", textureNameList.size());
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
      menu.TextfeldArray[T_FPS]->Positioniere(0,11.7,.3,A_LINKS);

    if (E_FullScreen==false &&
        E_ScreenXResolution==640 &&
        E_ColorDepth==16 &&
        KPConfig::Instance().TextureSize==8 &&
        KPConfig::Instance().MenuTextureSize==2 &&
        KPConfig::Instance().Reflections==false &&
        KPConfig::Instance().Shadows==false &&
        KPConfig::Instance().Nearest==true &&
        KPConfig::Instance().LightSources==2) {Quality=1;}
    else if (E_FullScreen==false &&
	     E_ScreenXResolution==640 &&
	     E_ColorDepth==16 &&
       KPConfig::Instance().TextureSize==4 &&
	     KPConfig::Instance().MenuTextureSize==2 &&
	     KPConfig::Instance().Reflections==false &&
	     KPConfig::Instance().Shadows==true &&
	     KPConfig::Instance().Nearest==true &&
	     KPConfig::Instance().LightSources==2) {Quality=2;}
    else if (E_FullScreen==false &&
	     E_ScreenXResolution==800 &&
	     E_ColorDepth==16 &&
       KPConfig::Instance().TextureSize==2 &&
	     KPConfig::Instance().MenuTextureSize==1 &&
	     KPConfig::Instance().Reflections==false &&
	     KPConfig::Instance().Shadows==true &&
	     KPConfig::Instance().Nearest==true &&
	     KPConfig::Instance().LightSources==2) {Quality=3;}
    else if (E_FullScreen==false &&
	     E_ScreenXResolution==1024 &&
	     E_ColorDepth==16 &&
       KPConfig::Instance().TextureSize==1 &&
	     KPConfig::Instance().MenuTextureSize==1 &&
	     KPConfig::Instance().Reflections==false &&
	     KPConfig::Instance().Shadows==true &&
	     KPConfig::Instance().Nearest==false &&
	     KPConfig::Instance().LightSources==3) {Quality=4;}
    else if (E_FullScreen==true &&
	     E_ScreenXResolution==1024 &&
	     E_ColorDepth==32 &&
       KPConfig::Instance().TextureSize==1 &&
	     KPConfig::Instance().MenuTextureSize==1 &&
	     KPConfig::Instance().Reflections==true &&
	     KPConfig::Instance().Shadows==true &&
	     KPConfig::Instance().Nearest==false &&
	     KPConfig::Instance().LightSources==3) {Quality=5;}
    else Quality=0;

    menu.SchildArray[SHLD_MENUBACKGROUND]->Positioniere(2,1.7,14,9.5);

    menu.SchildArray[SHLD_LOGO]->Positioniere(4,10,12,12);
    menu.SchildArray[SHLD_LOGO]->VollSichtbar();

    menu.TextfeldArray[T_EINSTELLUNGEN ]->Positioniere(4,9,1,A_LINKS);
    menu.TextfeldArray[T_EINSTELLUNGEN ]->VollSichtbar();
    menu.TextfeldArray[T_GRAFIK        ]->Positioniere(12,9,1,A_RECHTS);
    menu.TextfeldArray[T_GRAFIK        ]->VollSichtbar();

    y = 8.0;
    menu.TextfeldArray[T_QUALITAET     ]->Positioniere(8,y,1,A_RECHTS);
    menu.TextfeldArray[T_QUALITAET     ]->SetzeSignal(S_TOGGLE_QUALITY);
    y = 7.4;
    if (E_FullScreen == 0)
    {
      menu.TextfeldArray[T_AUFLOESUNG  ]->Positioniere(8,y,.6,A_RECHTS);
      menu.TextfeldArray[T_AUFLOESUNG  ]->SetzeSignal(S_TOGGLE_RESOLUTION);
    }
    y -= dy;
    menu.TextfeldArray[T_SCREENMODE    ]->Positioniere(8,y,.6,A_RECHTS);  y -= dy;
    menu.TextfeldArray[T_SCREENMODE    ]->SetzeSignal(S_TOGGLE_SCREENMODE);
    menu.TextfeldArray[T_MENUTEXTUREN  ]->Positioniere(8,y,.6,A_RECHTS);  y -= dy;
    menu.TextfeldArray[T_MENUTEXTUREN  ]->SetzeSignal(S_TOGGLE_MENUTEXTURES);
    menu.TextfeldArray[T_TEXTUREN      ]->Positioniere(8,y,.6,A_RECHTS); y -= dy;
    menu.TextfeldArray[T_TEXTUREN      ]->SetzeSignal(S_TOGGLE_TEXTURES);
    menu.TextfeldArray[T_TEXTURENAME   ]->Positioniere(8,y,.6,A_RECHTS);  y -= dy;
    menu.TextfeldArray[T_TEXTURENAME   ]->SetzeSignal(S_TOGGLE_TEXTURENAME);
    menu.TextfeldArray[T_REFLEKTIONEN  ]->Positioniere(8,y,.6,A_RECHTS);  y -= dy;
    menu.TextfeldArray[T_REFLEKTIONEN  ]->SetzeSignal(S_TOGGLE_REFLECTIONS);
    menu.TextfeldArray[T_FARBTIEFE     ]->Positioniere(8,y,.6,A_RECHTS);  y -= dy;
    menu.TextfeldArray[T_FARBTIEFE     ]->SetzeSignal(S_TOGGLE_COLORDEPTH);
    // Shadows currently not supported
	//menu.TextfeldArray[T_SCHATTEN    ]->Positioniere(8,y,.6,A_RECHTS);  y -= dy;
    //menu.TextfeldArray[T_SCHATTEN    ]->SetzeSignal(S_TOGGLE_SHADOWS);
    menu.TextfeldArray[T_TEXTURINTERPOLATION]->Positioniere(8,y,.6,A_RECHTS);  y -= dy;
    menu.TextfeldArray[T_TEXTURINTERPOLATION]->SetzeSignal(S_TOGGLE_TEXTUREINTERPOLATION);
    menu.TextfeldArray[T_AMBIENTESLICHT]->Positioniere(8,y,.6,A_RECHTS);  y -= dy;
    menu.TextfeldArray[T_AMBIENTESLICHT]->SetzeSignal(S_TOGGLE_AMBIENT_LIGHT);
    menu.TextfeldArray[T_TISCHLAMPEN   ]->Positioniere(8,y,.6,A_RECHTS);  y -= dy;
    menu.TextfeldArray[T_TISCHLAMPEN   ]->SetzeSignal(S_TOGGLE_LAMPS);
#if defined(HAVE_LIBGLUT) || defined(HAVE_LIBOPENGLUT)
    menu.TextfeldArray[T_USERINTERFACE ]->Positioniere(8,y,.6,A_RECHTS);  y -= dy;
    menu.TextfeldArray[T_USERINTERFACE ]->SetzeSignal(S_TOGGLE_USER_INTERFACE);
#endif
    menu.TextfeldArray[T_FRAMERATE     ]->Positioniere(8,y,.6,A_RECHTS);  y -= dy;
    menu.TextfeldArray[T_FRAMERATE     ]->SetzeSignal(S_TOGGLE_FPS);

    y = 8.0;
    switch (Quality) {
    case 1: {
      menu.TextfeldArray[T_Q_SEHRSCHNELL]->Positioniere(8.2,y,1,A_LINKS);
      menu.TextfeldArray[T_Q_SEHRSCHNELL]->SetzeSignal(S_TOGGLE_QUALITY);
    }break;
    case 2: {
      menu.TextfeldArray[T_Q_SCHNELL]->Positioniere(8.2,y,1,A_LINKS);
      menu.TextfeldArray[T_Q_SCHNELL]->SetzeSignal(S_TOGGLE_QUALITY);
    }break;
    case 3: {
      menu.TextfeldArray[T_Q_NORMAL]->Positioniere(8.2,y,1,A_LINKS);
      menu.TextfeldArray[T_Q_NORMAL]->SetzeSignal(S_TOGGLE_QUALITY);
    }break;
    case 4: {
      menu.TextfeldArray[T_Q_HOCH]->Positioniere(8.2,y,1,A_LINKS);
      menu.TextfeldArray[T_Q_HOCH]->SetzeSignal(S_TOGGLE_QUALITY);
    }break;
    case 5: {
      menu.TextfeldArray[T_Q_SEHRHOCH]->Positioniere(8.2,y,1,A_LINKS);
      menu.TextfeldArray[T_Q_SEHRHOCH]->SetzeSignal(S_TOGGLE_QUALITY);
    }break;
    default: {
      menu.TextfeldArray[T_Q_BENUTZER]->Positioniere(8.2,y,1,A_LINKS);
      menu.TextfeldArray[T_Q_BENUTZER]->SetzeSignal(S_TOGGLE_QUALITY);
    }break;
    }

    y = 7.4;
    if (!E_FullScreen) {
      switch (E_ScreenXResolution) {
      case 640: {
        menu.TextfeldArray[T_AUF_640x480]->Positioniere(8.2,y,.6,A_LINKS);
        menu.TextfeldArray[T_AUF_640x480]->SetzeSignal(S_TOGGLE_RESOLUTION);
      }break;
      case 1024: {
        menu.TextfeldArray[T_AUF_1024x768]->Positioniere(8.2,y,.6,A_LINKS);
        menu.TextfeldArray[T_AUF_1024x768]->SetzeSignal(S_TOGGLE_RESOLUTION);
      }break;
      case 1280: {
        menu.TextfeldArray[T_AUF_1280x960]->Positioniere(8.2,y,.6,A_LINKS);
        menu.TextfeldArray[T_AUF_1280x960]->SetzeSignal(S_TOGGLE_RESOLUTION);
      }break;
      case 1600: {
        menu.TextfeldArray[T_AUF_1600x1200]->Positioniere(8.2,y,.6,A_LINKS);
        menu.TextfeldArray[T_AUF_1600x1200]->SetzeSignal(S_TOGGLE_RESOLUTION);
      }break;
      default: {
        menu.TextfeldArray[T_AUF_800x600]->Positioniere(8.2,y,.6,A_LINKS);
        menu.TextfeldArray[T_AUF_800x600]->SetzeSignal(S_TOGGLE_RESOLUTION);
      }break;
      }
    }

    y -= dy;
    if (E_FullScreen)
    {
      menu.TextfeldArray[T_FULLSCREEN]->Positioniere(8.2,y,.6,A_LINKS);
      menu.TextfeldArray[T_FULLSCREEN]->SetzeSignal(S_TOGGLE_SCREENMODE);
    } else {
      menu.TextfeldArray[T_WINDOW]->Positioniere(8.2,y,.6,A_LINKS);
      menu.TextfeldArray[T_WINDOW]->SetzeSignal(S_TOGGLE_SCREENMODE);
    }

    y -= dy;
    switch (KPConfig::Instance().MenuTextureSize) {
    case 1: {
      menu.TextfeldArray[T_MT_HOCH]->Positioniere(8.2,y,.6,A_LINKS);
      menu.TextfeldArray[T_MT_HOCH]->SetzeSignal(S_TOGGLE_MENUTEXTURES);
    }break;
    case 2: {
      menu.TextfeldArray[T_MT_NORMAL]->Positioniere(8.2,y,.6,A_LINKS);
      menu.TextfeldArray[T_MT_NORMAL]->SetzeSignal(S_TOGGLE_MENUTEXTURES);
    }break;
    }

    y -= dy;
    switch (KPConfig::Instance().TextureSize) {
    case 1: {
      menu.TextfeldArray[T_T_HOCH]->Positioniere(8.2,y,.6,A_LINKS);
      menu.TextfeldArray[T_T_HOCH]->SetzeSignal(S_TOGGLE_TEXTURES);
    }break;
    case 2: {
      menu.TextfeldArray[T_T_NORMAL]->Positioniere(8.2,y,.6,A_LINKS);
      menu.TextfeldArray[T_T_NORMAL]->SetzeSignal(S_TOGGLE_TEXTURES);
    }break;
    case 4: {
      menu.TextfeldArray[T_T_NIEDRIG]->Positioniere(8.2,y,.6,A_LINKS);
      menu.TextfeldArray[T_T_NIEDRIG]->SetzeSignal(S_TOGGLE_TEXTURES);
    }break;
    case 8: {
      menu.TextfeldArray[T_T_SEHRNIEDRIG]->Positioniere(8.2,y,.6,A_LINKS);
      menu.TextfeldArray[T_T_SEHRNIEDRIG]->SetzeSignal(S_TOGGLE_TEXTURES);
    }break;
    }

    y -= dy;
    menu.TextfeldArray[T_TEXTURE1 + textureIndex]->Positioniere(8.2,y,.6,A_LINKS);
    menu.TextfeldArray[T_TEXTURE1 + textureIndex]->SetzeSignal(S_TOGGLE_TEXTURENAME);

    y -= dy;
    if (KPConfig::Instance().Reflections)
    {
      menu.TextfeldArray[T_REF_EIN]->Positioniere(8.2,y,.6,A_LINKS);
      menu.TextfeldArray[T_REF_EIN]->SetzeSignal(S_TOGGLE_REFLECTIONS);
    } else {
      menu.TextfeldArray[T_REF_AUS]->Positioniere(8.2,y,.6,A_LINKS);
      menu.TextfeldArray[T_REF_AUS]->SetzeSignal(S_TOGGLE_REFLECTIONS);
    }

    y -= dy;
    switch (E_ColorDepth) {
    case 32: {
      menu.TextfeldArray[T_FT_32]->Positioniere(8.2,y,.6,A_LINKS);
      menu.TextfeldArray[T_FT_32]->SetzeSignal(S_TOGGLE_COLORDEPTH);
    }break;
    default: {
      menu.TextfeldArray[T_FT_16]->Positioniere(8.2,y,.6,A_LINKS);
      menu.TextfeldArray[T_FT_16]->SetzeSignal(S_TOGGLE_COLORDEPTH);
    }break;
    }

    /*
	y -= dy;
    if (KPConfig::Instance().Shadows)
    {
      menu.TextfeldArray[T_S_EIN]->Positioniere(8.2,y,.6,A_LINKS);
      menu.TextfeldArray[T_S_EIN]->SetzeSignal(S_TOGGLE_SHADOWS);
    } else {
      menu.TextfeldArray[T_S_AUS]->Positioniere(8.2,y,.6,A_LINKS);
      menu.TextfeldArray[T_S_AUS]->SetzeSignal(S_TOGGLE_SHADOWS);
    }
    */

    y -= dy;
    if (KPConfig::Instance().Nearest)
    {
      menu.TextfeldArray[T_TI_AUS]->Positioniere(8.2,y,.6,A_LINKS);
      menu.TextfeldArray[T_TI_AUS]->SetzeSignal(S_TOGGLE_TEXTUREINTERPOLATION);
    } else {
      menu.TextfeldArray[T_TI_EIN]->Positioniere(8.2,y,.6,A_LINKS);
      menu.TextfeldArray[T_TI_EIN]->SetzeSignal(S_TOGGLE_TEXTUREINTERPOLATION);
    }

    y -= dy;
    if (KPConfig::Instance().AmbientLight)
    {
      menu.TextfeldArray[T_AMBIENTESLICHTEIN]->Positioniere(8.2,y,.6,A_LINKS);
      menu.TextfeldArray[T_AMBIENTESLICHTEIN]->SetzeSignal(S_TOGGLE_AMBIENT_LIGHT);
    } else {
      menu.TextfeldArray[T_AMBIENTESLICHTAUS]->Positioniere(8.2,y,.6,A_LINKS);
      menu.TextfeldArray[T_AMBIENTESLICHTAUS]->SetzeSignal(S_TOGGLE_AMBIENT_LIGHT);

    }

    y -= dy;
    switch (KPConfig::Instance().LightSources) {
    case 1: {
      menu.TextfeldArray[T_TISCHLAMPEN1]->Positioniere(8.2,y,.6,A_LINKS);
      menu.TextfeldArray[T_TISCHLAMPEN1]->SetzeSignal(S_TOGGLE_LAMPS);
    } break;
    case 2: {
      menu.TextfeldArray[T_TISCHLAMPEN2]->Positioniere(8.2,y,.6,A_LINKS);
      menu.TextfeldArray[T_TISCHLAMPEN2]->SetzeSignal(S_TOGGLE_LAMPS);
    } break;
    case 3: {
      menu.TextfeldArray[T_TISCHLAMPEN3]->Positioniere(8.2,y,.6,A_LINKS);
      menu.TextfeldArray[T_TISCHLAMPEN3]->SetzeSignal(S_TOGGLE_LAMPS);
    } break;
    }

#if defined(HAVE_LIBGLUT) || defined(HAVE_LIBOPENGLUT)
    y -= dy;
    switch (E_UserInterface) {
    case 0: {
      menu.TextfeldArray[T_USERINTERFACE0]->Positioniere(8.2,y,.6,A_LINKS);
      menu.TextfeldArray[T_USERINTERFACE0]->SetzeSignal(S_TOGGLE_USER_INTERFACE);
    } break;
    case 1: {
      menu.TextfeldArray[T_USERINTERFACE1]->Positioniere(8.2,y,.6,A_LINKS);
      menu.TextfeldArray[T_USERINTERFACE1]->SetzeSignal(S_TOGGLE_USER_INTERFACE);
    } break;
    }
#endif

    y -= dy;
    if (KPConfig::Instance().DisplayFPS)
    {
      menu.TextfeldArray[T_FRAMERATEEIN]->Positioniere(8.2,y,.6,A_LINKS);
      menu.TextfeldArray[T_FRAMERATEEIN]->SetzeSignal(S_TOGGLE_FPS);
    } else {
      menu.TextfeldArray[T_FRAMERATEAUS]->Positioniere(8.2,y,.6,A_LINKS);
      menu.TextfeldArray[T_FRAMERATEAUS]->SetzeSignal(S_TOGGLE_FPS);
    }

    menu.TextfeldArray[T_ZURUECK]->Positioniere(8,.7,1,A_MITTE);
    menu.TextfeldArray[T_ZURUECK]->SetzeSignal(S_BACK);

    StartAnimation();
}

void  KPstateGraphicSettings::MouseClick (KPstateContext *pContext, int button, int state, int x, int y)
{
  int Signal = KPstate::EvaluateMouseClick(pContext, button, state, x, y);

  switch (Signal) {
    case S_TOGGLE_FPS:                  ToggleFPS(pContext);                 UpdateDisplay(pContext); break;
    case S_TOGGLE_LAMPS:                ToggleLamps(pContext);               UpdateDisplay(pContext); break;
    case S_TOGGLE_REFLECTIONS:          ToggleReflections(pContext);         UpdateDisplay(pContext); break;
    case S_TOGGLE_SCREENMODE:           ToggleScreenMode(pContext);          UpdateDisplay(pContext); break;
    case S_TOGGLE_RESOLUTION:           ToggleResolution(pContext);          UpdateDisplay(pContext); break;
    case S_TOGGLE_COLORDEPTH:           ToggleColorDepth(pContext);          UpdateDisplay(pContext); break;
    case S_TOGGLE_SHADOWS:              ToggleShadows(pContext);             UpdateDisplay(pContext); break;
    case S_TOGGLE_TEXTUREINTERPOLATION: ToggleTextureInterpolation(pContext);UpdateDisplay(pContext); break;
    case S_TOGGLE_TEXTURES:             ToggleTextures(pContext);            UpdateDisplay(pContext); break;
    case S_TOGGLE_MENUTEXTURES:         ToggleMenuTextures(pContext);        UpdateDisplay(pContext); break;
    case S_TOGGLE_TEXTURENAME:          ToggleTextureName(pContext);         UpdateDisplay(pContext); break;
    case S_TOGGLE_AMBIENT_LIGHT:        ToggleAmbientLight(pContext);        UpdateDisplay(pContext); break;
#if defined(HAVE_LIBGLUT) || defined(HAVE_LIBOPENGLUT)
    case S_TOGGLE_USER_INTERFACE:       ToggleUserInterface(pContext);       UpdateDisplay(pContext); break;
#endif
    case S_TOGGLE_QUALITY:              ToggleQuality(pContext);             UpdateDisplay(pContext); break;

    case S_BACK:                        {
                                          tKPMenuState newState = SaveChanges(pContext);
                                          pContext->ChangeState(newState);
  }                                     }
}

void KPstateGraphicSettings::KeyPressed (KPstateContext *pContext, unsigned char key, int x, int y)
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
      ResolutionChanged = true;

    DEBUGPRINT("Changed Graphic Settings\n");
    DEBUGPRINT1("  UserInterface :        %s\n", KPConfig::GetUserInterfaceName(E_UserInterface));
    DEBUGPRINT1("  TextureName :          %s\n", (const char *)KPConfig::Instance().TextureName);
    DEBUGPRINT1("  TextureSize :          %d\n", KPConfig::Instance().TextureSize);
    DEBUGPRINT1("  MenuTextureSize :      %d\n", KPConfig::Instance().MenuTextureSize);
    DEBUGPRINT1("  Shadows :              %s\n", KPConfig::Instance().Shadows ? "On" : "Off");
    DEBUGPRINT1("  TextureInterpolation : %s\n", KPConfig::Instance().Nearest ? "Off" : "On");
    DEBUGPRINT1("  ScreenXResolution :    %d\n", E_ScreenXResolution);
    DEBUGPRINT1("  ColorDepth :           %d\n", E_ColorDepth);
    DEBUGPRINT1("  Reflections :          %s\n", KPConfig::Instance().Reflections ? "On" : "Off");
    DEBUGPRINT1("  AmbientLight :         %s\n", KPConfig::Instance().AmbientLight ? "On" : "Off");
    DEBUGPRINT1("  LightSources :         %d\n", KPConfig::Instance().LightSources);
    DEBUGPRINT1("  FullScreen :           %s\n", E_FullScreen ? "On" : "Off");

    KPConfig::Instance().ColorDepth        = E_ColorDepth;
    KPConfig::Instance().FullScreen        = E_FullScreen;
    KPConfig::Instance().ScreenXResolution = E_ScreenXResolution;
    KPConfig::Instance().UserInterface     = E_UserInterface;

    KPConfig::Instance().WriteToFile();

    if (ResolutionChanged)
      return KPState_GraphicHint;
    else
      return oldStateId;
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
  switch (KPConfig::Instance().LightSources) {
    case 1:   KPConfig::Instance().LightSources = 2; break;
    case 2:   KPConfig::Instance().LightSources = 3; break;
    case 3:   KPConfig::Instance().LightSources = 1; break;
    default:  KPConfig::Instance().LightSources = 1; break;
  }
  pContext->GetLight().Update(KPConfig::Instance().AmbientLight,
                              KPConfig::Instance().LightSources,
                              KPConfig::Instance().Reflections);
}

void KPstateGraphicSettings::ToggleTextures(KPstateContext *pContext)
{
  pContext->GetUserInterface().PlayAudio(KP_SND_CHANGESETTING);
  switch (KPConfig::Instance().TextureSize) {
    case 1: KPConfig::Instance().TextureSize  = 2; break;
    case 2: KPConfig::Instance().TextureSize  = 4; break;
    case 4: KPConfig::Instance().TextureSize  = 8; break;
    case 8: KPConfig::Instance().TextureSize  = 1; break;
    default: KPConfig::Instance().TextureSize = 1; break;
    }
    pContext->GetBoardView().InitializeTextures( KPConfig::Instance().TextureName,
                                                 KPConfig::Instance().TextureSize,
                                                 KPConfig::Instance().Nearest );
}

void KPstateGraphicSettings::ToggleTextureName(KPstateContext *pContext)
{
  if (textureNameList.size() > 1)
  {
	  std::vector<BString>::const_iterator it;

    it = std::find(textureNameList.begin(), textureNameList.end(), KPConfig::Instance().TextureName);
    if (it == textureNameList.end())
      it = textureNameList.begin();
    else
    {
      ++it;
      if (it == textureNameList.end())
        it = textureNameList.begin();
    }
    KPConfig::Instance().TextureName = *it;
    textureIndex = GetTextureIndex(KPConfig::Instance().TextureName);
    pContext->GetBoardView().InitializeTextures( KPConfig::Instance().TextureName,
                                                 KPConfig::Instance().TextureSize,
                                                 KPConfig::Instance().Nearest,
                                                 false );

    pContext->GetMenu().Update( KPConfig::Instance().TextureName,
                                KPConfig::Instance().MenuTextureSize,
                                KPConfig::Instance().Nearest,
                                false );
    pContext->GetUserInterface().InitializeAudio(KPConfig::Instance().TextureName, true);

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
  switch (E_ScreenXResolution) {
    case 640:  E_ScreenXResolution = 800;  break;
    case 800:  E_ScreenXResolution = 1024; break;
    case 1024: E_ScreenXResolution = 1280; break;
    case 1280: E_ScreenXResolution = 1600; break;
    case 1600: E_ScreenXResolution = 640;  break;
    default:   E_ScreenXResolution = 800;  break;
  }
}

void KPstateGraphicSettings::ToggleColorDepth(KPstateContext *pContext)
{
  pContext->GetUserInterface().PlayAudio(KP_SND_CHANGESETTING);
  switch (E_ColorDepth) {
    case 16: E_ColorDepth = 32; break;
    default: E_ColorDepth = 16; break;
  }
}

void KPstateGraphicSettings::ToggleUserInterface(KPstateContext *pContext)
{
  pContext->GetUserInterface().PlayAudio(KP_SND_CHANGESETTING);
  switch (E_UserInterface) {
    case 0:  E_UserInterface = 1; break;
    case 1:  E_UserInterface = 0; break;
    default: E_UserInterface = 0; break;
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
  switch (KPConfig::Instance().MenuTextureSize) {
    case 1:  KPConfig::Instance().MenuTextureSize = 2; break;
    case 2:  KPConfig::Instance().MenuTextureSize = 1; break;
    default: KPConfig::Instance().MenuTextureSize = 1; break;
  }
  pContext->GetMenu().Update( KPConfig::Instance().TextureName,
                              KPConfig::Instance().MenuTextureSize,
                              KPConfig::Instance().Nearest );
}

void KPstateGraphicSettings::ToggleTextureInterpolation(KPstateContext *pContext)
{
  pContext->GetUserInterface().PlayAudio(KP_SND_CHANGESETTING);
  KPConfig::Instance().Nearest = !KPConfig::Instance().Nearest;
  pContext->GetMenu().Update( KPConfig::Instance().TextureName,
                              KPConfig::Instance().MenuTextureSize,
                              KPConfig::Instance().Nearest );
  pContext->GetBoardView().InitializeTextures( KPConfig::Instance().TextureName,
                                               KPConfig::Instance().TextureSize,
                                               KPConfig::Instance().Nearest );
}

void KPstateGraphicSettings::ToggleQuality(KPstateContext *pContext)
{
  pContext->GetUserInterface().PlayAudio(KP_SND_CHANGESETTING);
  switch(Quality) {
    case 1: {
      E_FullScreen=true;
      E_ScreenXResolution=1024;
      E_ColorDepth=32;
      KPConfig::Instance().TextureSize=1;
      KPConfig::Instance().MenuTextureSize=1;
      KPConfig::Instance().Reflections=true;
      KPConfig::Instance().Shadows=true;
      KPConfig::Instance().Nearest=false;
      KPConfig::Instance().LightSources=3;
    } break;
    case 2: {
      E_FullScreen=false;
      E_ScreenXResolution=640;
      E_ColorDepth=16;
      KPConfig::Instance().TextureSize=8;
      KPConfig::Instance().MenuTextureSize=2;
      KPConfig::Instance().Reflections=false;
      KPConfig::Instance().Shadows=false;
      KPConfig::Instance().Nearest=true;
      KPConfig::Instance().LightSources=2;
    } break;
    case 3: {
      E_FullScreen=false;
      E_ScreenXResolution=640;
      E_ColorDepth=16;
      KPConfig::Instance().TextureSize=4;
      KPConfig::Instance().MenuTextureSize=2;
      KPConfig::Instance().Reflections=false;
      KPConfig::Instance().Shadows=true;
      KPConfig::Instance().Nearest=true;
      KPConfig::Instance().LightSources=2;
    } break;
    case 4: {
      E_FullScreen=false;
      E_ScreenXResolution=800;
      E_ColorDepth=16;
      KPConfig::Instance().TextureSize=2;
      KPConfig::Instance().MenuTextureSize=1;
      KPConfig::Instance().Reflections=false;
      KPConfig::Instance().Shadows=true;
      KPConfig::Instance().Nearest=true;
      KPConfig::Instance().LightSources=2;
    } break;
    case 5: {
      E_FullScreen=false;
      E_ScreenXResolution=1024;
      E_ColorDepth=16;
      KPConfig::Instance().TextureSize=1;
      KPConfig::Instance().MenuTextureSize=1;
      KPConfig::Instance().Reflections=false;
      KPConfig::Instance().Shadows=true;
      KPConfig::Instance().Nearest=false;
      KPConfig::Instance().LightSources=3;
    } break;
    default: {
      E_FullScreen=false;
      E_ScreenXResolution=800;
      E_ColorDepth=16;
      KPConfig::Instance().TextureSize=2;
      KPConfig::Instance().MenuTextureSize=1;
      KPConfig::Instance().Reflections=false;
      KPConfig::Instance().Shadows=true;
      KPConfig::Instance().Nearest=true;
      KPConfig::Instance().LightSources=2;
    } break;
  }
  pContext->GetLight().Update(KPConfig::Instance().AmbientLight,
                              KPConfig::Instance().LightSources,
                              KPConfig::Instance().Reflections);
  pContext->GetMenu().Update( KPConfig::Instance().TextureName,
                              KPConfig::Instance().MenuTextureSize,
                              KPConfig::Instance().Nearest );
  pContext->GetBoardView().InitializeTextures( KPConfig::Instance().TextureName,
                                               KPConfig::Instance().TextureSize,
                                               KPConfig::Instance().Nearest );
}

int KPstateGraphicSettings::GetTextureIndex(BString &TextureName)
{
  int index = 0;
  
  std::vector<BString>::const_iterator it = textureNameList.begin();

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
