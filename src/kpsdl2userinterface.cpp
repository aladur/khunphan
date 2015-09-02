/*
    kpsdluserinterface.cpp

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

#include "misc1.h"

#ifdef HAVE_SDL2

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <io.h>
#endif
#include "misc1.h"
#ifdef HAVE_UNISTD_H
  #include <sys/types.h>
  #include <unistd.h>  // needed for access
#endif
#include <limits.h>
#include "kpsdl2userinterface.h"
#include "btime.h"
#include "bdir.h"


const char *KPSdl2UserInterface::soundFile[KP_SND_MAX+1] =
{
  "openmenu.ogg",      // KP_SND_OPENMENU
  "quitgame.ogg",      // KP_SND_QUITGAME
  "movetoken.ogg",     // KP_SND_MOVETOKEN
  "changesetting.ogg", // KP_SND_CHANGESETTING
  "tutorialnext.ogg",  // KP_SND_TUTORIALNEXT
  "opengame.ogg",      // KP_SND_OPENGAME
  "gamesolved.ogg",    // KP_SND_GAMESOLVED
  NULL
};
  
KPSdl2UserInterface::KPSdl2UserInterface() : window(NULL), sound(NULL), soundSource(NULL),
                                           music(NULL), rate(22050), musicIndex(0)
{
}

KPSdl2UserInterface::~KPSdl2UserInterface()
{
  if (window != NULL)
  {
     window = NULL;
     SDL_DestroyRenderer(renderer);
     SDL_DestroyWindow(window);
     SDL_Quit();
  }

  /*
  if (sound != NULL)
  {
    for (int i = 0; i < KP_SND_MAX; i++)
      if (sound[i] != NULL)
        Mix_FreeChunk(sound[i]);
    delete [] sound;
    sound = NULL;
  }
  if (soundSource != NULL)
  {
    delete [] soundSource;
    soundSource = NULL;
  }
  if (music != NULL)
     Mix_FreeMusic(music);
  music = NULL;
  */
}

/////////////////////////////////////////////////////////////////////
// Public Interface
/////////////////////////////////////////////////////////////////////

void KPSdl2UserInterface::InitializeEvents()
{
  // nothing to do here
}

bool KPSdl2UserInterface::CanToggleFullScreen() const
{
  return true;
}

void KPSdl2UserInterface::SetWindowMode(bool isfullscreen) const
{
  if (window == NULL || !CanToggleFullScreen())
  {
    return;
  }

  SDL_SetWindowFullscreen(window,
                          isfullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0 );
}

bool KPSdl2UserInterface::OpenWindow(int /* argc */ , char ** /* argv */)
{
  char title[64];
  int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
  SDL_version compiled;
  SDL_version linked;
  const SDL_version *pVersion;

  SDL_VERSION(&compiled);
  SDL_GetVersion(&linked);

  DEBUGPRINT("SDL UserInterface initialization\n");
  DEBUGPRINT3("SDL linked version: %d.%d.%d\n",
              linked.major, linked.minor, linked.patch);
  DEBUGPRINT3("SDL compiled version: %d.%d.%d\n",
              compiled.major, compiled.minor, compiled.patch);
  DEBUGPRINT3("SDL Header version: %d.%d.%d\n", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);
  pVersion = Mix_Linked_Version();
  DEBUGPRINT3("SDL_mixer Linked version: %d.%d.%d\n", pVersion->major, pVersion->minor, pVersion->patch);
  DEBUGPRINT3("SDL_mixer Header version: %d.%d.%d\n", MIX_MAJOR_VERSION, MIX_MINOR_VERSION, MIX_PATCHLEVEL);

  // Open OpenGL Window with SDL
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_NOPARACHUTE) < 0)
  {
    return false;
  }

  if (KPConfig::Instance().FullScreen)
  {
    flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
  }

  sprintf(title, "%s V%s", PACKAGE, VERSION);

  window = SDL_CreateWindow(
         title,
         SDL_WINDOWPOS_UNDEFINED,
         SDL_WINDOWPOS_UNDEFINED,
         KPConfig::Instance().ScreenXResolution,
         (KPConfig::Instance().ScreenXResolution*3)/4,
         flags);

  if (window == NULL)
  {
    printf("*** SDL_CreateWindow Error: %s\n", SDL_GetError());
    SDL_Quit();
    return false;
  }

  renderer = SDL_CreateRenderer(window, -1, 0);
  if (renderer == NULL)
  {
    printf("*** SDL_CreateRenderer Error: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return false;
  }

  DebugPrintOpenGLVersion();
  InitializeAudio(KPConfig::Instance().TextureName.c_str());
  return InitializeAfterOpen();
}

void KPSdl2UserInterface::MainLoop()
{
  // This is the event loop
  bool done = 0;
  unsigned char key;

  while ( ! done )
  {
    SDL_Event event;

    memset(&event, 0, sizeof(event));

    Idle();
    
    while ( SDL_PollEvent(&event) )
    {
      switch(event.type)
      {
        case SDL_WINDOWEVENT:
        switch (event.window.event)
        {
          case SDL_WINDOWEVENT_SIZE_CHANGED:
            Reshape(event.window.data1, event.window.data2);
            break;
        }
        break;
        case SDL_QUIT:
          done = 1;
          break;
        case SDL_MOUSEMOTION:
          MouseMotion(event.motion.x, event.motion.y);
          break;
        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEBUTTONDOWN:
          MouseClick(event.button.button, event.button.state,
                     event.button.x, event.button.y);
          break;
        case SDL_KEYDOWN:
          if (mapKey(event.key.keysym.mod, event.key.keysym.sym, &key))
          {
            int xm, ym;
            SDL_GetMouseState(&xm, &ym);
            KeyPressed(key, xm, ym);
          }
          break;
        case SDL_KEYUP:
          if (mapKey(event.key.keysym.mod, event.key.keysym.sym, &key))
          {
            int xm, ym;
            SDL_GetMouseState(&xm, &ym);
            KeyReleased(key, xm, ym);
          }
          break;
      } // switch
    } // event loop
    Display();
    PostWindowRedisplay();

    SDL_Delay(10);
  } // main loop
}

bool KPSdl2UserInterface::mapKey(int mod, int sym, unsigned char *pKey)
{
  // this is only a hack for key mapping compatible to GLUT
  if (mod & (KMOD_LCTRL | KMOD_RCTRL | KMOD_CTRL))
    {
    *pKey = (unsigned char)sym & 0x1F;
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
    return false;

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
  
void KPSdl2UserInterface::Close()
{
  if (window != NULL)
  {
     SDL_DestroyRenderer(renderer);
     SDL_DestroyWindow(window);
     window = NULL;
     SDL_Quit();
  }
  KPConfig::Instance().WriteToFile();

  exit(0);
}

int KPSdl2UserInterface::GetValue(int what) const
{
  if (window != NULL)
  {
    int width;
    int height;

    SDL_GetWindowSize(window, &width, &height);

    switch (what)
    {
      case KP_WINDOW_WIDTH:  return width;
      case KP_WINDOW_HEIGHT: return height;
    }
  }

  return 0;
}


/////////////////////////////////////////////////////////////////////
// Event Handling
/////////////////////////////////////////////////////////////////////

void KPSdl2UserInterface::SwapBuffers()
{
  if (window != NULL)
  {
     SDL_GL_SwapWindow(window);
  }
}

void KPSdl2UserInterface::Timer(int)
{
  // unused
}

void KPSdl2UserInterface::PostWindowRedisplay()
{
  // Is there something to do here?
}

void KPSdl2UserInterface::MouseClick( int button, int state, int x, int y )
{
  int kpState, kpButton;

  switch (button)
  {
    case SDL_BUTTON_LEFT:    kpButton = KP_LEFT_MB;   break;
    case SDL_BUTTON_MIDDLE:  kpButton = KP_MIDDLE_MB; break;
    case SDL_BUTTON_RIGHT:   kpButton = KP_RIGHT_MB;  break;
    default:                 kpButton = 0;
  }
  switch (state)
  {
    case SDL_PRESSED:    kpState = KP_BUTTON_PRESS;   break;
    case SDL_RELEASED:   kpState = KP_BUTTON_RELEASE; break;
    default:             kpState = 0;
  }
  pState->MouseClick(this, kpButton, kpState, x, y);
}

/////////////////////////////////////////////////////////////////////
// Audio/Music Interface
/////////////////////////////////////////////////////////////////////

bool KPSdl2UserInterface::InitializeAudio(const char *textureName, bool reInitialize /* = false */)
{
  if(!reInitialize) DEBUGPRINT("Audio and Music initialization\n");
  
  if(!reInitialize && Mix_OpenAudio(rate, AUDIO_S16, 1, 4096))
  {
    DEBUGPRINT1("*** Error in Mix_OpenAudio: %s\n", Mix_GetError());
    return false;
  }

  std::string file1, file2;
  int     i;

  if (sound == NULL)
  {
    // Initialize sound management variables
    sound            = new Mix_Chunk *[KP_SND_MAX];
    soundSource      = new std::string [KP_SND_MAX];
    for (i = 0; i < KP_SND_MAX; i++)
      sound[i] = NULL;
  }

  i = 0;
  Mix_HaltChannel(-1);
  Mix_HaltMusic();

  while (soundFile[i] != NULL)
  {
    file1 = KPConfig::Instance().GetDirectory(KP_SOUND_DIR) + textureName +
                                PATHSEPARATORSTRING + soundFile[i];
    file2 = KPConfig::Instance().GetDirectory(KP_SOUND_DIR) + soundFile[i];

    if (soundSource[i] == file1 ||
        (access(file1.c_str(), R_OK) && soundSource[i] == file2))
      { i++; continue; }; // right file already prepared to be used
      
    if (sound[i] != NULL)
    {
      Mix_FreeChunk(sound[i]);
      sound[i] = NULL;
    }
    soundSource[i] = "";

    if (!access(file1.c_str(), R_OK))
      soundSource[i] = file1;
    else
      if(!access(file2.c_str(), R_OK))
        soundSource[i] = file2;
      else
        DEBUGPRINT1("*** Warning: No sound file available for sound '%s'\n", soundFile[i]);

    i++;
  }

  if (!reInitialize)
  {
    SetSoundVolume(KPConfig::Instance().SoundVolume);

    musicFiles = BDirectory::GetFiles(KPConfig::Instance().GetDirectory(KP_MUSIC_DIR));
    std::sort(musicFiles.begin(), musicFiles.end());

    musicIndex = 0;
  }

  return true;
}

void KPSdl2UserInterface::LoadNextMusic()
{
  if (music != NULL)
  {
    Mix_FreeMusic(music);
    music = NULL;
  }

  while (true)
  {
    if (musicFiles.empty())
      return;

    int index = 0;
	std::vector<std::string>::iterator it = musicFiles.begin();
    while (it != musicFiles.end() && index != musicIndex)
    {
      ++it; ++index;
    }
    if (it == musicFiles.end())
    {
      index = 0;
      it = musicFiles.begin();
    }

    std::string file = KPConfig::Instance().GetDirectory(KP_MUSIC_DIR) + *it;
    if ((music = Mix_LoadMUS(file.c_str())) == NULL)
    {
      DEBUGPRINT2("*** Error in Mix_LoadMUS(\"%s\"): %s\n",
                  file.c_str(), Mix_GetError());
      musicFiles.erase(it);
      continue;
    }
    else
    {
      DEBUGPRINT1("Loading '%s'\n", file.c_str());
      musicIndex = index + 1;
      SetMusicVolume(KPConfig::Instance().MusicVolume);
      break;
    }
  }
}
  
void KPSdl2UserInterface::PlayAudio(int soundId) const
{
  if (sound == NULL || soundId < 0 || soundId >= KP_SND_MAX)
    return;
 
  // Lazy initialization: Load the sound when playing first time
  if (sound[soundId] == NULL && soundSource != NULL && !soundSource[soundId].empty())
  {
    if ((sound[soundId] = Mix_LoadWAV_RW(
        SDL_RWFromFile(soundSource[soundId].c_str(), "rb"), 1)) == NULL)
    {
       DEBUGPRINT2("*** Error opening Audio file '%s' [%s]\n",
          soundSource[soundId].c_str(), Mix_GetError());
    }
    if (sound[soundId] != NULL)
       DEBUGPRINT1("Reading '%s'\n", soundSource[soundId].c_str());
  }

  if (sound[soundId] != NULL)
    Mix_PlayChannel(-1, sound[soundId], 0);
}

void KPSdl2UserInterface::SetSoundVolume(int volume) const
{
  if (sound != NULL)
    Mix_Volume(-1, MIX_MAX_VOLUME * volume / 100);
}

void KPSdl2UserInterface::SetMusicVolume(int volume) const
{
  if (music != NULL)
    Mix_VolumeMusic(MIX_MAX_VOLUME * volume / 100);
}

void KPSdl2UserInterface::PlayMusic(bool On, bool resetPos)
{
  static double pos = 0.0;
  static BTime time;
  
  if (On == true)
  {
    if (music == NULL) LoadNextMusic();

    if (music != NULL && !Mix_PlayingMusic())
    {
      time.ResetRelativeTime();
      Mix_FadeInMusicPos(music, 1, 1000, pos);
      Mix_HookMusicFinished(stopMusicCallback);
    }
  }
  if (music != NULL && On == false && Mix_PlayingMusic())
  {
    pos = time.GetRelativeTimeUsf(true) / 1000000.0; // relative position in seconds
    Mix_HookMusicFinished(NULL);
    Mix_FadeOutMusic(1000);
  }
  if (resetPos == true)
    pos = 0.0;
}

void KPSdl2UserInterface::StopMusicCallback()
{
  if (music != NULL)
  {
    Mix_FreeMusic(music);
    music = NULL;
  }
  PlayMusic(true, true);
}
  
void KPSdl2UserInterface::stopMusicCallback()
{
  if (KPSdl2UserInterface::instance != NULL)
    KPSdl2UserInterface::instance->StopMusicCallback();  
}

#endif //#ifdef HAVE_SDL2

