/*
    kpsdluserinterface.cpp

    Automatic solution finder for KhunPhan game
    Copyright (C) 2001-2024  W. Schwotzer

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

#include <limits.h>
#include <string>
#include <locale>
#include <algorithm>
#include <cassert>
#include "kpsdluserinterface.h"
#include "bdir.h"
#include "kpconfig.h"
#include "kpstate.h"
#include "blogger.h"


const int KPSdlUserInterface::REQUEST_FOR_CLOSE = 99;

const KPSdlUserInterface::tArrayOfString KPSdlUserInterface::soundFiles =
{
    "openmenu.ogg",      // KPSound::OpenMenu
    "quitgame.ogg",      // KPSound::QuitGame
    "movetoken.ogg",     // KPSound::MoveToken
    "changesetting.ogg", // KPSound::ChangeSetting
    "tutorialnext.ogg",  // KPSound::TutorialNext
    "opengame.ogg",      // KPSound::OpenGame
    "gamesolved.ogg"     // KPSound::GameSolved
};

KPSdlUserInterface::KPSdlUserInterface(KPConfigPtr PConfig) :
    KPUIBase(PConfig),
    music(nullptr), rate(22050), musicIndex(0), musicPosition(0.0)
{
    std::fill(sounds.begin(), sounds.end(), nullptr);
}

KPSdlUserInterface::~KPSdlUserInterface()
{
    CloseAudio();
    Close();
}

void KPSdlUserInterface::CloseAudio()
{
    for (auto sound : sounds)
    {
        if (sound != nullptr)
        {
            Mix_FreeChunk(sound);
            sound = nullptr;
        }
    }

    if (music != nullptr)
    {
        Mix_FreeMusic(music);
        music = nullptr;
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

void KPSdlUserInterface::MouseClick(int button, int event, int x, int y)
{
    MouseButtonEvent kpEvent;
    MouseButton kpButton;

    switch (button)
    {
        case SDL_BUTTON_LEFT:
            kpButton = MouseButton::Left;
            break;

        case SDL_BUTTON_MIDDLE:
            kpButton = MouseButton::Middle;
            break;

        case SDL_BUTTON_RIGHT:
            kpButton = MouseButton::Right;
            break;

        default:
            return;
    }

    switch (event)
    {
        case SDL_PRESSED:
            kpEvent = MouseButtonEvent::Press;
            break;

        case SDL_RELEASED:
            kpEvent = MouseButtonEvent::Release;
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
    user_event.user.data1 = nullptr;
    user_event.user.data2 = nullptr;
    SDL_PushEvent(&user_event);
}

void KPSdlUserInterface::SetStopMusicCallback()
{
    Mix_HookMusicFinished(StaticStopMusicCallback);
}

void KPSdlUserInterface::StaticStopMusicCallback()
{
    if (KPSdlUserInterface::pInstance != nullptr)
    {
        KPSdlUserInterface::pInstance->StopMusicCallback();
    }
}

/////////////////////////////////////////////////////////////////////
// Audio/Music Interface
/////////////////////////////////////////////////////////////////////

bool KPSdlUserInterface::InitializeAudio(const std::string &textureName)
{
    BLogger::Log("Audio and Music initialization");

    if (Mix_OpenAudio(rate, AUDIO_S16, 1, 4096))
    {
        BLogger::Log("*** Error in Mix_OpenAudio: ", Mix_GetError());
        return false;
    }

    Mix_HaltChannel(-1);
    Mix_HaltMusic();

    assert(sounds.size() == soundSources.size());

    for (decltype(sounds.size()) idx = 0; idx < sounds.size(); ++idx)
    {
        if (sounds[idx] != nullptr)
        {
            Mix_FreeChunk(sounds[idx]);
            sounds[idx] = nullptr;
        }

        soundSources[idx] = "";

        auto file = config->Get(KPDirectory::Sound) + textureName +
                    PATHSEPARATORSTRING + soundFiles[idx];

        sounds[idx] = Mix_LoadWAV_RW(
                          SDL_RWFromFile(file.c_str(), "rb"), 1);

        if (sounds[idx] == nullptr)
        {
            file = config->Get(KPDirectory::Sound) + soundFiles[idx];
            sounds[idx] = Mix_LoadWAV_RW(
                              SDL_RWFromFile(file.c_str(), "rb"), 1);
        }

        if (sounds[idx] != nullptr)
        {
            soundSources[idx] = file;
            BLogger::Log("Reading '", soundSources[idx], "'");
        }
        else
        {
            BLogger::Log("*** Error opening sound file '",
                         soundSources[idx], "' [", Mix_GetError(), "]");
        }
    }

    SetSoundVolume(config->SoundVolume);

    musicFiles = BDirectory::GetFiles(config->Get(KPDirectory::Music));
    auto newEndIt =
        std::remove_if(musicFiles.begin(), musicFiles.end(), NoMusicFile);
    musicFiles.erase(newEndIt, musicFiles.end());
    std::sort(musicFiles.begin(), musicFiles.end());

    musicIndex = 0;

    return true;
}

bool KPSdlUserInterface::NoMusicFile(const std::string &file)
{
    auto fileExtension = file.substr(file.size() - 4, 4);
    std::locale locale;

    for (auto &character : fileExtension)
    {
        character = std::tolower(character, locale);
    }

    return fileExtension.compare(".ogg") && fileExtension.compare(".mp3");
}

void KPSdlUserInterface::LoadNextMusic()
{
    if (music != nullptr)
    {
        Mix_FreeMusic(music);
        music = nullptr;
    }

    if (!musicFiles.empty())
    {
        while (true)
        {
            auto it = musicFiles.begin();

            if (musicIndex >= musicFiles.size())
            {
                musicIndex = 0;
            }

            it += musicIndex;

            auto file = config->Get(KPDirectory::Music) + *it;

            if ((music = Mix_LoadMUS(file.c_str())) == nullptr)
            {
                BLogger::Log("*** Error in Mix_LoadMUS(\"", file, "\"): ",
                             Mix_GetError());
                musicFiles.erase(it);
                continue;
            }
            else
            {
                BLogger::Log("Loading '", file, "'");
                musicIndex++;
                SetMusicVolume(config->MusicVolume);
                break;
            }
        }
    }
}

void KPSdlUserInterface::PlayAudio(KPSound soundId) const
{
    std::size_t index = static_cast<std::size_t>(soundId);

    if (index < sounds.size() && (sounds[index] != nullptr))
    {
        Mix_PlayChannel(-1, sounds[index], 0);
    }
}

void KPSdlUserInterface::SetSoundVolume(int volume) const
{
    Mix_Volume(-1, MIX_MAX_VOLUME * volume / 100);
}

void KPSdlUserInterface::SetMusicVolume(int volume) const
{
    if (music != nullptr)
    {
        Mix_VolumeMusic(MIX_MAX_VOLUME * volume / 100);
    }
}

void KPSdlUserInterface::PlayMusic(bool On, bool resetPos)
{
    if (On)
    {
        if (music == nullptr)
        {
            LoadNextMusic();
        }

        if (music != nullptr && !Mix_PlayingMusic())
        {
            // Fade in music at a previously stored position or at 0.0
            time.ResetRelativeTime();
            Mix_FadeInMusicPos(music, 1, 1000, musicPosition);
            SetStopMusicCallback();
        }
    }

    if (music != nullptr && !On && Mix_PlayingMusic())
    {
        // Music switched off: Get relative position in seconds.
        musicPosition = time.GetRelativeTimeUsf(true) / 1000000.0;
        Mix_HookMusicFinished(nullptr);
        Mix_FadeOutMusic(1000);
    }

    if (resetPos)
    {
        musicPosition = 0.0;
    }
}

void KPSdlUserInterface::StopMusicCallback()
{
    if (music != nullptr)
    {
        Mix_FreeMusic(music);
        music = nullptr;
    }

    PlayMusic(true, true);
}

#endif //#if defined (HAVE_SDL) || defined (HAVE_SDL2)
