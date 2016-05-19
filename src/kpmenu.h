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
#include "kpconfig.h"


enum class Signal;

enum class KPPlate
{
    MenuBackground,
    Logo,
    SoundOn,
    SoundOff,
    MusicOn,
    MusicOff,
    Shader // should be always the last one
};

typedef std::map<Lbl, Label> tIdToLabel;
typedef std::map<KPPlate, Plate> tIdToPlate;

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

    KPmenu(KPConfigPtr PConfig);
    KPmenu() = delete;
    KPmenu(const KPmenu &) = delete;
    KPmenu &operator=(const KPmenu &) = delete;

    void Initialize(std::string &TextureName, int TextureSize, bool Nearest,
                    Signal Language);
    void Update(std::string &TextureName, int TextureSize, bool Nearest);
    void Draw();
    void UpdateFPS(int fps, float renderTime = 0.0);
    void SaveLastStateId(StateId stateId)
    {
        lastState = stateId;
    };
    StateId RestoreLastStateId(void) const
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
    bool LoadLanguage(Signal Language);
    void AddOrSetLabel(Lbl labelId, const std::string &text);
    void FadeOutAllPlates();
    void FadeOutAllLabels();
    void Animate(unsigned int duration);
    void UpdatePlayTime(KPstateContext *pContext, unsigned int duration);

    tIdToPlate plates;
    tIdToLabel labels;
    StateId lastState;
    BManualTimer playTimeUpdateTimer;
    KPConfigPtr config;
    bool isDisplayOpenGLInfo;
};

#endif
