/*
    kpsdluserinterface.cpp

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
#if defined (HAVE_SDL) || defined (HAVE_SDL2)

#ifdef HAVE_UNISTD_H
#include <sys/types.h>
#include <unistd.h>  // needed for access
#endif
#include <limits.h>
#include <string>
#include <locale>
#include "kpsdluserinterface.h"
#include "bdir.h"
#include "kpconfig.h"
#include "kpstate.h"


const int KPSdlUserInterface::REQUEST_FOR_CLOSE = 99;

const char *KPSdlUserInterface::soundFile[KP_SND_MAX] =
{
    "openmenu.ogg",      // KP_SND_OPENMENU
    "quitgame.ogg",      // KP_SND_QUITGAME
    "movetoken.ogg",     // KP_SND_MOVETOKEN
    "changesetting.ogg", // KP_SND_CHANGESETTING
    "tutorialnext.ogg",  // KP_SND_TUTORIALNEXT
    "opengame.ogg",      // KP_SND_OPENGAME
    "gamesolved.ogg"     // KP_SND_GAMESOLVED
};

KPSdlUserInterface::KPSdlUserInterface(KPConfig &Config) :
    KPUIBase(Config),
    sound(NULL), soundSource(NULL),
    music(NULL), rate(22050), musicIndex(0), musicPosition(0.0)
{
}

KPSdlUserInterface::~KPSdlUserInterface()
{
    CloseAudio();
    Close();
}

void KPSdlUserInterface::CloseAudio()
{
    if (sound != NULL)
    {
        for (int i = 0; i < KP_SND_MAX; i++)
        {
            if (sound[i] != NULL)
            {
                Mix_FreeChunk(sound[i]);
            }
         }
        delete [] sound;
        sound = NULL;
    }
    if (soundSource != NULL)
    {
        delete [] soundSource;
        soundSource = NULL;
    }
    if (music != NULL)
    {
        Mix_FreeMusic(music);
        music = NULL;
    }
}

void KPSdlUserInterface::Close()
{
}

/////////////////////////////////////////////////////////////////////
// Public Interface
/////////////////////////////////////////////////////////////////////

void KPSdlUserInterface::InitializeEvents()
{
    // nothing to do here
}

bool KPSdlUserInterface::mapKey(int mod, int sym, unsigned char *pKey)
{
    // this is only a hack for key mapping compatible to GLUT
    if (mod & (KMOD_LCTRL | KMOD_RCTRL | KMOD_CTRL))
    {
        *pKey = static_cast<unsigned char>(sym) & 0x1F;
        return true;
    }
    if ((mod & (KMOD_LSHIFT | KMOD_RSHIFT | KMOD_SHIFT)) &&
        sym >= SDLK_a  && sym <= SDLK_z)
    {
        *pKey = sym - 32;
        return true;
    }
    if (mod & (KMOD_LSHIFT | KMOD_RSHIFT | KMOD_SHIFT |
               KMOD_LCTRL  | KMOD_RCTRL  |
               KMOD_LALT   | KMOD_RALT))
    {
        return false;
    }

    if (sym == SDLK_ESCAPE)
    {
        *pKey = 0x1b;
        return true;
    }
    if (sym == SDLK_BACKSPACE)
    {
        *pKey = 0x08;
        return true;
    }
    if (sym == SDLK_RETURN)
    {
        *pKey = 0x0D;
        return true;
    }
    if (sym >= SDLK_SPACE && sym <= SDLK_z)
    {
        *pKey = sym;
        return true;
    }
    return false;
}

/////////////////////////////////////////////////////////////////////
// Event Handling
/////////////////////////////////////////////////////////////////////

void KPSdlUserInterface::PostWindowRedisplay()
{
    // Is there something to do here?
}

void KPSdlUserInterface::MouseClick( int button, int event, int x, int y )
{
    tMouseEvent  kpEvent;
    tMouseButton kpButton;

    switch (button)
    {
        case SDL_BUTTON_LEFT:
            kpButton = KP_LEFT_MB;
            break;
        case SDL_BUTTON_MIDDLE:
            kpButton = KP_MIDDLE_MB;
            break;
        case SDL_BUTTON_RIGHT:
            kpButton = KP_RIGHT_MB;
            break;
        default:
            return;
    }

    switch (event)
    {
        case SDL_PRESSED:
            kpEvent = KP_BUTTON_PRESS;
            break;
        case SDL_RELEASED:
            kpEvent = KP_BUTTON_RELEASE;
            break;
        default:
            return;
    }

    pState->MouseClick(this, kpButton, kpEvent, x, y);
}

void KPSdlUserInterface::RequestForClose()
{
    SDL_Event user_event;

    user_event.type = SDL_USEREVENT;
    user_event.user.code = REQUEST_FOR_CLOSE;
    user_event.user.data1 = NULL;
    user_event.user.data2 = NULL;
    SDL_PushEvent(&user_event);
}

/////////////////////////////////////////////////////////////////////
// Audio/Music Interface
/////////////////////////////////////////////////////////////////////

bool KPSdlUserInterface::InitializeAudio(const char *textureName,
        bool reInitialize /* = false */)
{
    if (!reInitialize)
    {
        LOG1("Audio and Music initialization");
    }

    if (!reInitialize && Mix_OpenAudio(rate, AUDIO_S16, 1, 4096))
    {
        LOG2("*** Error in Mix_OpenAudio: ", Mix_GetError());
        return false;
    }

    std::string file1, file2;
    int i;

    if (sound == NULL)
    {
        // Initialize sound management variables
        sound            = new Mix_Chunk *[KP_SND_MAX];
        soundSource      = new std::string [KP_SND_MAX];
        for (i = 0; i < KP_SND_MAX; i++)
        {
            sound[i] = NULL;
        }
    }

    i = 0;
    Mix_HaltChannel(-1);
    Mix_HaltMusic();

    while (i < KP_SND_MAX)
    {
        file1 = config.GetDirectory(KP_SOUND_DIR) + textureName +
                PATHSEPARATORSTRING + soundFile[i];
        file2 = config.GetDirectory(KP_SOUND_DIR) + soundFile[i];

        if (soundSource[i] == file1 ||
            (access(file1.c_str(), R_OK) && soundSource[i] == file2))
        {
            i++;
            continue;
        }; // right file already prepared to be used

        if (sound[i] != NULL)
        {
            Mix_FreeChunk(sound[i]);
            sound[i] = NULL;
        }
        soundSource[i] = "";

        if (!access(file1.c_str(), R_OK))
        {
            soundSource[i] = file1;
        }
        else if (!access(file2.c_str(), R_OK))
        {
            soundSource[i] = file2;
        }
        else
        {
            LOG3("**** Warning: No sound file available for sound '",
                 soundFile[i], "'");
        }

        i++;
    }

    if (!reInitialize)
    {
        SetSoundVolume(config.SoundVolume);

        musicFiles = BDirectory::GetFiles(config.GetDirectory(KP_MUSIC_DIR));
        std::remove_if(musicFiles.begin(), musicFiles.end(), IsMusicFile);
        std::sort(musicFiles.begin(), musicFiles.end());

        musicIndex = 0;
    }

    return true;
}

bool KPSdlUserInterface::IsMusicFile(const std::string file)
{
    std::string fileExtension = file.substr(file.size() - 4, 4);
    std::string::iterator it = fileExtension.begin();
    std::locale locale;

    for (; it != fileExtension.end(); ++it)
    {
        *it = std::tolower(*it, locale);
    }

    return fileExtension.compare(".ogg") || fileExtension.compare(".mp3");
}

void KPSdlUserInterface::LoadNextMusic()
{
    if (music != NULL)
    {
        Mix_FreeMusic(music);
        music = NULL;
    }

    if (!musicFiles.empty())
    {
        while (true)
        {
            std::vector<std::string>::iterator it = musicFiles.begin();
            if (musicIndex >= musicFiles.size())
            {
                musicIndex = 0;
            }
            it += musicIndex;

            std::string file = config.GetDirectory(KP_MUSIC_DIR) + *it;
            if ((music = Mix_LoadMUS(file.c_str())) == NULL)
            {
                LOG4("*** Error in Mix_LoadMUS(\"", file, "\"): ",
                    Mix_GetError());
                musicFiles.erase(it);
                continue;
            }
            else
            {
                LOG3("Loading '", file, "'");
                musicIndex++;
                SetMusicVolume(config.MusicVolume);
                break;
            }
        }
    }
}

void KPSdlUserInterface::PlayAudio(int soundId) const
{
    if (sound == NULL || soundId < 0 || soundId >= KP_SND_MAX)
    {
        return;
    }

    // Lazy initialization: Load the sound when playing first time
    if (sound[soundId] == NULL && soundSource != NULL &&
        !soundSource[soundId].empty())
    {
        if ((sound[soundId] = Mix_LoadWAV_RW(
                                  SDL_RWFromFile(soundSource[soundId].c_str(),
                                          "rb"), 1)) == NULL)
        {
            LOG5("*** Error opening Audio file '", soundSource[soundId],
                 "' [", Mix_GetError(), "]");
        }
        if (sound[soundId] != NULL)
        {
            LOG3("Reading '", soundSource[soundId], "'");
        }
    }

    if (sound[soundId] != NULL)
    {
        Mix_PlayChannel(-1, sound[soundId], 0);
    }
}

void KPSdlUserInterface::SetSoundVolume(int volume) const
{
    if (sound != NULL)
    {
        Mix_Volume(-1, MIX_MAX_VOLUME * volume / 100);
    }
}

void KPSdlUserInterface::SetMusicVolume(int volume) const
{
    if (music != NULL)
    {
        Mix_VolumeMusic(MIX_MAX_VOLUME * volume / 100);
    }
}

void KPSdlUserInterface::PlayMusic(bool On, bool resetPos)
{
    if (On)
    {
        if (music == NULL)
        {
            LoadNextMusic();
        }

        if (music != NULL && !Mix_PlayingMusic())
        {
            // Fade in music at a previously stored position or at 0.0
            time.ResetRelativeTime();
            Mix_FadeInMusicPos(music, 1, 1000, musicPosition);
            SetStopMusicCallback();
        }
    }

    if (music != NULL && !On && Mix_PlayingMusic())
    {
        // Music switched off: Get relative position in seconds.
        musicPosition = time.GetRelativeTimeUsf(true) / 1000000.0;
        Mix_HookMusicFinished(NULL);
        Mix_FadeOutMusic(1000);
    }

    if (resetPos)
    {
        musicPosition = 0.0;
    }
}

void KPSdlUserInterface::StopMusicCallback()
{
    if (music != NULL)
    {
        Mix_FreeMusic(music);
        music = NULL;
    }
    PlayMusic(true, true);
}

#endif //#if defined (HAVE_SDL) || defined (HAVE_SDL2)
