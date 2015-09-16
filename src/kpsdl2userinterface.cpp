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


KPSdl2UserInterface::KPSdl2UserInterface() :
    KPSdlUserInterface(), window(NULL), renderer(NULL)
{
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
    DEBUGPRINT3("SDL Header version: %d.%d.%d\n", SDL_MAJOR_VERSION,
                SDL_MINOR_VERSION, SDL_PATCHLEVEL);
    pVersion = Mix_Linked_Version();
    DEBUGPRINT3("SDL_mixer Linked version: %d.%d.%d\n", pVersion->major,
                pVersion->minor, pVersion->patch);
    DEBUGPRINT3("SDL_mixer Header version: %d.%d.%d\n", MIX_MAJOR_VERSION,
                MIX_MINOR_VERSION, MIX_PATCHLEVEL);

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

