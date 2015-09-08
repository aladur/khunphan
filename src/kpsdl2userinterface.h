/*
    kpsdluserinterface.h

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

#ifndef KPSDL2USERINTERFACE_H
#define KPSDL2USERINTERFACE_H

#include "misc1.h"
#ifdef HAVE_SDL2

#include <SDL.h>
#include <SDL_mixer.h>
#include <algorithm>
#include <vector>
#include "kpuibase.h"
#include "kpconfig.h"
#include "kpstate.h"


class KPSdl2UserInterface : public KPUIBase {
public:
  // public interface
  KPSdl2UserInterface();
  virtual ~KPSdl2UserInterface();

  bool OpenWindow(int argc, char **argv);
  void SetWindowMode(bool FullScreen) const;
  void MainLoop();
  void Close();
  int  GetValue(int what) const;
  bool CanToggleFullScreen() const;

protected:
  // member functions for event handling
  void Timer(int value);
  void MouseClick(int button, int state, int x, int y);

  void InitializeEvents();
  void SwapBuffers();
  void PostWindowRedisplay();
  bool mapKey(int mod, int sym, unsigned char *pKey);
  Mix_Chunk *LoadSoundFile(const char *pFile);
  void StopMusicCallback();

  static void stopMusicCallback();

  SDL_Window   *window;  // SDL Window ID
  SDL_Renderer *renderer;
  Mix_Chunk    **sound;
  std::string  *soundSource;
  Mix_Music    *music;

// Audio/Music Interface
public:
  bool InitializeAudio(const char *textureName, bool reInitialize = false);
  void SetSoundVolume(int volume) const;
  void SetMusicVolume(int volume) const;
  void PlayAudio(int soundId) const;
  void PlayMusic(bool On, bool resetPos = false);
  void LoadNextMusic();

private:
  unsigned int rate;
  int musicIndex;
  std::vector<std::string> musicFiles;
  static const char *soundFile[KP_SND_MAX+1];
};

#endif //#ifdef HAVE_SDL2
#endif