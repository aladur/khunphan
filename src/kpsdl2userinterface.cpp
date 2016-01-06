/*
    kpsdl2userinterface.cpp

    Automatic solution finder for KhunPhan game
    Copyright (C) 2015  W. Schwotzer

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

#include "kpsdl2userinterface.h"


KPSdl2UserInterface::KPSdl2UserInterface(KPboard &rootBoard) :
    KPSdlUserInterface(),
    window(NULL), renderer(NULL)
{
    Initialize(rootBoard);
}

KPSdl2UserInterface::~KPSdl2UserInterface()
{
    if (window != NULL)
    {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;
        SDL_DestroyWindow(window);
        window = NULL;
        SDL_Quit();
    }
}

/////////////////////////////////////////////////////////////////////
// Public Interface
/////////////////////////////////////////////////////////////////////

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
    KPConfig &config = KPConfig::Instance();
    int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    SDL_version compiled;
    SDL_version linked;
    const SDL_version *pVersion;

    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);

    LOG1("SDL UserInterface initialization");
    LOG6("SDL linked version: ",
         static_cast<unsigned int>(linked.major), '.',
         static_cast<unsigned int>(linked.minor), '.',
         static_cast<unsigned int>(linked.patch));
    LOG6("SDL compiled version: ",
         static_cast<unsigned int>(compiled.major), '.',
         static_cast<unsigned int>(compiled.minor), '.',
         static_cast<unsigned int>(compiled.patch));
    LOG6("SDL Header version: ",
         SDL_MAJOR_VERSION, '.', SDL_MINOR_VERSION, '.', SDL_PATCHLEVEL);
    pVersion = Mix_Linked_Version();
    LOG6("SDL_mixer Linked version: ",
         static_cast<unsigned int>(pVersion->major), '.',
         static_cast<unsigned int>(pVersion->minor), '.',
         static_cast<unsigned int>(pVersion->patch));
    LOG6("SDL_mixer Header version: ",
         MIX_MAJOR_VERSION, '.', MIX_MINOR_VERSION, '.', MIX_PATCHLEVEL);

    // Open OpenGL Window with SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_NOPARACHUTE) < 0)
    {
        return false;
    }

    if (config.FullScreen)
    {
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }

    window = SDL_CreateWindow(
                 GetWindowTitle().c_str(),
                 SDL_WINDOWPOS_UNDEFINED,
                 SDL_WINDOWPOS_UNDEFINED,
                 config.ScreenXResolution,
                 (config.ScreenXResolution*3)/4,
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
    InitializeAudio(config.TextureName.c_str());
    InitializeAfterOpen();

    return true;
}

void KPSdl2UserInterface::MainLoop()
{
    // This is the event loop
    bool done = false;
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
                    if (mapKey(event.key.keysym.mod, event.key.keysym.sym,
                               &key))
                    {
                        int xm, ym;
                        SDL_GetMouseState(&xm, &ym);
                        KeyPressed(key, xm, ym);
                    }
                    break;
                case SDL_KEYUP:
                    if (mapKey(event.key.keysym.mod, event.key.keysym.sym,
                               &key))
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

void KPSdl2UserInterface::Close()
{
    if (window != NULL)
    {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;
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
            case KP_WINDOW_WIDTH:
                return width;
            case KP_WINDOW_HEIGHT:
                return height;
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

/////////////////////////////////////////////////////////////////////
// Audio/Music Interface
/////////////////////////////////////////////////////////////////////

void KPSdl2UserInterface::SetStopMusicCallback()
{
    Mix_HookMusicFinished(stopMusicCallback);
}

void KPSdl2UserInterface::stopMusicCallback()
{
    if (KPSdl2UserInterface::instance != NULL)
    {
        KPSdl2UserInterface::instance->StopMusicCallback();
    }
}

#endif //#ifdef HAVE_SDL2

