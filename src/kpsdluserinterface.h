/*
    kpsdluserinterface.h

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

#ifndef KPSDLUSERINTERFACE_H
#define KPSDLUSERINTERFACE_H

#include "stdafx.h"
#if defined (HAVE_SDL) || defined (HAVE_SDL2)

#include <SDL.h>
#include <SDL_mixer.h>
#include <vector>
#include <array>
#include <algorithm>
#include "kpuibase.h"
#include "btime.h"
#include "kpconfig.h"


class KPSdlUserInterface : public KPUIBase
{
public:
    // public interface
    KPSdlUserInterface(KPConfigPtr);
    virtual ~KPSdlUserInterface();
    KPSdlUserInterface() = delete;
    KPSdlUserInterface(const KPSdlUserInterface &) = delete;
    KPSdlUserInterface & operator=(const KPSdlUserInterface &) = delete;

    void Close() override;
    virtual void RequestForClose() override;

    // member functions for event handling
    void MouseClick(int button, int state, int x, int y) override;

protected:
    void InitializeEvents() override;
    void PostWindowRedisplay() override;

    bool mapKey(int mod, int sym, unsigned char *pKey);

    // Audio/Music Interface
public:
    bool InitializeAudio(const std::string &textureName) override;
    void SetSoundVolume(int volume) const override;
    void SetMusicVolume(int volume) const override;
    void PlayAudio(size_t soundId) const override;
    void PlayMusic(bool On, bool resetPos = false) override;
    void StopMusicCallback() override;

protected:
    Mix_Chunk *LoadSoundFile(const char *pFile);
    void SetStopMusicCallback();
    void LoadNextMusic();
    void CloseAudio();

    static void StaticStopMusicCallback();
    static bool NoMusicFile(const std::string &file);
    static const int REQUEST_FOR_CLOSE;

    std::array<Mix_Chunk *, KP_SND_MAX> sounds;
    std::array<std::string, KP_SND_MAX> soundSources;
    Mix_Music *music;

    typedef std::array<std::string, KP_SND_MAX> tArrayOfString;

    unsigned int rate;
    unsigned int musicIndex;
    std::vector<std::string> musicFiles;
    double musicPosition; // save position of music in seconds
    BTime time;
    static const tArrayOfString soundFiles;
};

#endif //#if defined (HAVE_SDL) || defined (HAVE_SDL2)
#endif
