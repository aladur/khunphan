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


class KPSdlUserInterface : public KPUIBase
{
public:
    // public interface
    KPSdlUserInterface(KPConfig &);
    virtual ~KPSdlUserInterface();

    void Close() override;

protected:
    // member functions for event handling
    void Timer(int value);
    void MouseClick(int button, int event, int x, int y);

    void InitializeEvents();
    void PostWindowRedisplay();
    bool mapKey(int mod, int sym, unsigned char *pKey);
    Mix_Chunk *LoadSoundFile(const char *pFile);
    void StopMusicCallback();
    virtual void SetStopMusicCallback() = 0;
    virtual void RequestForClose() override;
    void CloseAudio();

    std::array<Mix_Chunk *, KP_SND_MAX> sounds;
    std::array<std::string, KP_SND_MAX> soundSources;
    Mix_Music    *music;

    // Audio/Music Interface
public:
    bool InitializeAudio(const char *textureName);
    void SetSoundVolume(int volume) const;
    void SetMusicVolume(int volume) const;
    void PlayAudio(size_t soundId) const;
    void PlayMusic(bool On, bool resetPos = false);
    void LoadNextMusic();

protected:
    static const int REQUEST_FOR_CLOSE;

private:
    typedef std::array<std::string, KP_SND_MAX> tArrayOfString;

    KPSdlUserInterface();
    static bool NoMusicFile(const std::string &file);

    unsigned int rate;
    unsigned int musicIndex;
    std::vector<std::string> musicFiles;
    double musicPosition; // save position of music in seconds
    BTime time;
    static const tArrayOfString soundFiles;
};

#endif //#if defined (HAVE_SDL) || defined (HAVE_SDL2)
#endif
