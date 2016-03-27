/*
    kpmenu.h

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

#ifndef KPMENU_H
#define KPMENU_H

#include <map>
#include "plate.h"
#include "label.h"
#include "language.h"
#include "kpstate.h"
#include "bmanualtimer.h"


typedef enum
{
    PLATE_MENUBACKGROUND,
    PLATE_LOGO,
    PLATE_SOUND_ON,
    PLATE_SOUND_OFF,
    PLATE_MUSIC_ON,
    PLATE_MUSIC_OFF,
    PLATE_SHADER // should be always the last one
} tPlate;

typedef enum
{
    S_INVALID                     = -1,
    S_LOGO                        = 100,
    S_BACK                        = 101,
    S_SAVECHANGES                 = 102,
    S_CONTINUE                    = 103,

    S_NEW_GAME                    = 110,
    S_TUTORIAL                    = 111,
    S_SETTINGS                    = 112,
    S_FINISH                      = 113,
    S_SELECT_LANGUAGE             = 114,
    S_GRAPHIC_SETTINGS            = 115,
    S_CONTROL_SETTINGS            = 116,
    S_AUDIO_SETTINGS              = 117,
    S_KEYBOARD_HELP               = 118,
    S_SCORELIST                   = 119,

    S_TOGGLE_LAMPS                = 130,
    S_TOGGLE_TEXTURENAME          = 131,
    S_TOGGLE_RESOLUTION           = 133,
    S_TOGGLE_TEXTUREINTERPOL      = 136,
    S_TOGGLE_MENUTEXTURES         = 137,
    S_TOGGLE_TEXTURES             = 138,
    S_TOGGLE_QUALITY              = 139,
    S_TOGGLE_FPS                  = 140,
    S_TOGGLE_SOLUTION_HINT        = 141,
    S_TOGGLE_MOUSE_SPEED          = 142,
    S_TOGGLE_AMBIENT_LIGHT        = 143,
    S_TOGGLE_SCREENMODE           = 144,
    S_TOGGLE_SOUND_VOLUME         = 145,
    S_TOGGLE_MUSIC_VOLUME         = 146,
    S_TOGGLE_USER_INTERFACE       = 147,

    S_TOGGLE_SOUND_ON             = 160,
    S_TOGGLE_MUSIC_ON             = 161
} tSignal;

typedef std::map<int, Label>      tIdToLabel;
typedef std::map<tPlate, Plate>   tIdToPlate;

class KPstate;

class KPmenu
{
public:
    friend class KPstate;
    friend class KPstateStartUp;
    friend class KPstateMainMenu;
    friend class KPstateFinish;
    friend class KPstateSettings;
    friend class KPstateControlSettings;
    friend class KPstateGraphicSettings;
    friend class KPstateGraphicHint;
    friend class KPstateAudioSettings;
    friend class KPstateHelp;
    friend class KPstateKeyboardHelp;
    friend class KPstateGame;
    friend class KPstateSelectLanguage;
    friend class KPstateGameSolved;
    friend class KPstateTutorial1;
    friend class KPstateTutorial2;
    friend class KPstateTutorial3;
    friend class KPstateTutorial4;
    friend class KPstateScoreList;
    friend class KPstateLightTest;

    KPmenu(KPConfig &Config);

    void Initialize(std::string &TextureName, int TextureSize, bool Nearest,
                    int Language = 0);
    void Update(std::string &TextureName, int TextureSize, bool Nearest);
    void Draw();
    void UpdateFPS(int fps, float renderTime = 0.0);
    void SaveLastStateId(tKPMenuState stateId)
    {
        lastState = stateId;
    };
    tKPMenuState RestoreLastStateId(void) const
    {
        return lastState;
    };
    bool GetIsDisplayOpenGLInfo(void) const
    {
        return isDisplayOpenGLInfo;
    }
    void SetIsDisplayOpenGLInfo(bool flag)
    {
        isDisplayOpenGLInfo = flag;
    }
    void InvertIsDisplayOpenGLInfo(void)
    {
        isDisplayOpenGLInfo = !isDisplayOpenGLInfo;
    }

protected:
    bool LoadLanguage(int Language);
    void AddOrSetLabel(int number, const std::string &text);
    void FadeOutAllPlates();
    void FadeOutAllLabels();
    void Animate(unsigned int duration);
    void UpdatePlayTime(KPstateContext *pContext, unsigned int duration);

    tIdToPlate plates;
    tIdToLabel labels;
    tKPMenuState lastState;
    BManualTimer playTimeUpdateTimer;
    KPConfig &config;
    bool isDisplayOpenGLInfo;

private:
    KPmenu();
};

#endif
