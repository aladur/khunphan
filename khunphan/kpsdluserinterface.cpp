/*
    kpsdluserinterface.cpp

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

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <io.h>
#endif
#include "misc1.h"
#ifdef HAVE_UNISTD_H
  #include <unistd.h>  // needed for access
#endif
#include <limits.h>
#include "kpsdluserinterface.h"
#include "btime.h"
#include "bdir.h"


const char *KPSdlUserInterface::soundFile[KP_SND_MAX+1] =
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
  
KPSdlUserInterface::KPSdlUserInterface() : screen(NULL), sound(NULL), soundSource(NULL),
                                           music(NULL), rate(22050), musicIndex(0)
{
}

KPSdlUserInterface::~KPSdlUserInterface()
{
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
}

/////////////////////////////////////////////////////////////////////
// Public Interface
/////////////////////////////////////////////////////////////////////

void KPSdlUserInterface::InitializeEvents()
{
  // nothing to do here
}

void KPSdlUserInterface::SetWindowMode(bool /* FullScreen */) const
{
  if (screen == NULL)
    return;

  SDL_WM_ToggleFullScreen(screen);
}

bool KPSdlUserInterface::OpenWindow(int /* argc */ , char ** /* argv */)
{
  char temp[64];
  int flags = SDL_OPENGL | SDL_RESIZABLE;
  const SDL_version *pVersion;

  DEBUGPRINT("SDL UserInterface initialization\n");
  pVersion = SDL_Linked_Version();
  DEBUGPRINT3("SDL Linked version: %d.%d.%d\n", pVersion->major, pVersion->minor, pVersion->patch);
  DEBUGPRINT3("SDL Header version: %d.%d.%d\n", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);
  pVersion = Mix_Linked_Version();
  DEBUGPRINT3("SDL_mixer Linked version: %d.%d.%d\n", pVersion->major, pVersion->minor, pVersion->patch);
  DEBUGPRINT3("SDL_mixer Header version: %d.%d.%d\n", MIX_MAJOR_VERSION, MIX_MINOR_VERSION, MIX_PATCHLEVEL);

  // Open OpenGL Window with SDL
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_NOPARACHUTE) < 0)
    return false;

  if (KPConfig::Instance().FullScreen)
    flags |= SDL_FULLSCREEN;

  screen = SDL_SetVideoMode(
         KPConfig::Instance().ScreenXResolution,
         (KPConfig::Instance().ScreenXResolution*3)/4,
         KPConfig::Instance().ColorDepth,
         flags);
  if (screen == NULL)
  {
    SDL_Quit();
    return false;
  }
  atexit(SDL_Quit);

  sprintf(temp, "%s V%s", PACKAGE, VERSION);
  SDL_WM_SetCaption(temp, temp);

  DebugPrintOpenGLVersion();
  InitializeAudio(KPConfig::Instance().TextureName);
  return InitializeAfterOpen();
}

void KPSdlUserInterface::MainLoop()
{
  // This is the event loop
  bool done = 0;
  unsigned char key;

  while ( ! done )
  {
    SDL_Event event;

    Idle();
    
    while ( SDL_PollEvent(&event) )
    {
      switch(event.type)
      {
        case SDL_VIDEORESIZE:
          screen = SDL_SetVideoMode(event.resize.w, event.resize.h,
                                    KPConfig::Instance().ColorDepth,
                                    SDL_OPENGL | SDL_RESIZABLE);
          if ( screen ) {
            Reshape(screen->w, screen->h);
          } else {
            // unable to set video mode -> Quit
            done = 1;
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

bool KPSdlUserInterface::mapKey(int mod, int sym, unsigned char *pKey)
{
  // this is only a hack for key mapping compatible to GLUT
  if (sym == SDLK_LCTRL || sym == KMOD_RCTRL  || sym == KMOD_CTRL  ||
  sym == SDLK_LSHIFT    || sym == KMOD_RSHIFT || sym == KMOD_SHIFT ||
  sym == SDLK_LSUPER    || sym == KMOD_CAPS   || sym == KMOD_RALT  ||
  sym == SDLK_LALT      || sym == SDLK_RSUPER )
  {
    return false;
  }
  if (mod == KMOD_LCTRL ||
      mod == KMOD_RCTRL ||
      mod == KMOD_CTRL)
    {
    *pKey = (unsigned char)sym & 0x1F;
    return true;
  }
  if (mod == KMOD_NONE  &&
      sym >= SDLK_SPACE &&
      sym <= SDLK_z)
    {
    *pKey = sym;
    return true;
  }
  if ((mod == KMOD_LSHIFT ||
      mod == KMOD_RSHIFT ||
      mod == KMOD_SHIFT) &&
      sym >= SDLK_a  &&
      sym <= SDLK_z)
    {
    *pKey = sym - 32;
    return true;
  }
  if (mod == KMOD_NONE &&
      sym == SDLK_ESCAPE)
  {
    *pKey = 0x1b;
    return true;
  }
  if (mod == KMOD_NONE &&
      sym == SDLK_BACKSPACE)
  {
    *pKey = 0x08;
    return true;
  }
  if (mod == KMOD_NONE &&
      sym == SDLK_RETURN)
  {
    *pKey = 0x0D;
    return true;
  }
  return true;
}
  
void KPSdlUserInterface::Close()
{
  screen = NULL;
  KPConfig::Instance().WriteToFile();
  // The Destruction of the singleton instances
  // is done with the atexit() function
  exit(0);
}

int KPSdlUserInterface::GetValue(int what) const
{
  switch (what)
  {
    case KP_WINDOW_WIDTH:  return (screen != NULL ? screen->w : 0);
    case KP_WINDOW_HEIGHT: return (screen != NULL ? screen->h : 0);
  }
  return 0;
}


/////////////////////////////////////////////////////////////////////
// Event Handling
/////////////////////////////////////////////////////////////////////

void KPSdlUserInterface::SwapBuffers()
{
  SDL_GL_SwapBuffers();
}

void KPSdlUserInterface::Timer(int)
{
  // unused
}

void KPSdlUserInterface::PostWindowRedisplay()
{
  // Is there something to do here?
}

void KPSdlUserInterface::MouseClick( int button, int state, int x, int y )
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

bool KPSdlUserInterface::InitializeAudio(const char *textureName, bool reInitialize /* = false */)
{
  if(!reInitialize) DEBUGPRINT("Audio and Music initialization\n");
  
  if(!reInitialize && Mix_OpenAudio(rate, AUDIO_S16, 1, 4096))
  {
    DEBUGPRINT1("*** Error in Mix_OpenAudio: %s\n", Mix_GetError());
    return false;
  }

  BString file1, file2;
  int     i;

  if (sound == NULL)
  {
    // Initialize sound management variables
    sound            = new Mix_Chunk *[KP_SND_MAX];
    soundSource      = new BString [KP_SND_MAX];
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

    if (soundSource[i] == file1 || (access(file1, R_OK) && soundSource[i] == file2))
      { i++; continue; }; // right file already prepared to be used
      
    if (sound[i] != NULL)
    {
      Mix_FreeChunk(sound[i]);
      sound[i] = NULL;
    }
    soundSource[i] = "";

    if (!access(file1, R_OK))
      soundSource[i] = file1;
    else
      if(!access(file2, R_OK))
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

void KPSdlUserInterface::LoadNextMusic()
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
	std::vector<BString>::iterator it = musicFiles.begin();
    while (it != musicFiles.end() && index != musicIndex)
    {
      it++; index++;
    }
    if (it == musicFiles.end())
    {
      index = 0;
      it = musicFiles.begin();
    }

    BString file = KPConfig::Instance().GetDirectory(KP_MUSIC_DIR) + *it;
    if ((music = Mix_LoadMUS(file)) == NULL)
    {
      DEBUGPRINT2("*** Error in Mix_LoadMUS(\"%s\"): %s\n", (const char *)file, Mix_GetError());
      musicFiles.erase(it);
      continue;
    }
    else
    {
      DEBUGPRINT1("Loading '%s'\n", (const char *)file);
      musicIndex = index + 1;
      SetMusicVolume(KPConfig::Instance().MusicVolume);
      break;
    }
  }
}
  
void KPSdlUserInterface::PlayAudio(int soundId) const
{
  if (sound == NULL || soundId < 0 || soundId >= KP_SND_MAX)
    return;
 
  // Lazy initialization: Load the sound when playing first time
  if (sound[soundId] == NULL && soundSource != NULL && !soundSource[soundId].empty())
  {
    if ((sound[soundId] = Mix_LoadWAV_RW(SDL_RWFromFile(soundSource[soundId], "rb"), 1)) == NULL)
    {
       DEBUGPRINT2("*** Error opening Audio file '%s' [%s]\n", (const char *)soundSource[soundId], Mix_GetError());
    }
    if (sound[soundId] != NULL) DEBUGPRINT1("Reading '%s'\n", (const char *)soundSource[soundId]);
  }

  if (sound[soundId] != NULL)
    Mix_PlayChannel(-1, sound[soundId], 0);
}

void KPSdlUserInterface::SetSoundVolume(int volume) const
{
  if (sound != NULL)
    Mix_Volume(-1, MIX_MAX_VOLUME * volume / 100);
}

void KPSdlUserInterface::SetMusicVolume(int volume) const
{
  if (music != NULL)
    Mix_VolumeMusic(MIX_MAX_VOLUME * volume / 100);
}

void KPSdlUserInterface::PlayMusic(bool On, bool resetPos)
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

void KPSdlUserInterface::StopMusicCallback()
{
  if (music != NULL)
  {
    Mix_FreeMusic(music);
    music = NULL;
  }
  PlayMusic(true, true);
}
  
void KPSdlUserInterface::stopMusicCallback()
{
  if (KPSdlUserInterface::instance != NULL)
    KPSdlUserInterface::instance->StopMusicCallback();  
}
