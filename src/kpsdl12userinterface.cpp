/*
    kpsdl12userinterface.cpp

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
#ifdef HAVE_SDL

#include "kpsdl12userinterface.h"


KPSdl12UserInterface::KPSdl12UserInterface() :
    KPSdlUserInterface(), screen(NULL)
{
}

KPSdl12UserInterface::~KPSdl12UserInterface()
{
    if ( screen )
    {
        SDL_Quit();
        screen = NULL;
    }
}

/////////////////////////////////////////////////////////////////////
// Public Interface
/////////////////////////////////////////////////////////////////////

void KPSdl12UserInterface::SetWindowMode(bool /* FullScreen */) const
{
    if (screen == NULL || !CanToggleFullScreen())
    {
        return;
    }

    SDL_WM_ToggleFullScreen(screen);
}

bool KPSdl12UserInterface::OpenWindow(int /* argc */ , char ** /* argv */)
{
    char temp[64];
    int flags = SDL_OPENGL | SDL_RESIZABLE;
    const SDL_version *pVersion;

    DEBUGPRINT("SDL UserInterface initialization\n");
    pVersion = SDL_Linked_Version();
    DEBUGPRINT3("SDL Linked version: %d.%d.%d\n", pVersion->major,
                pVersion->minor, pVersion->patch);
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
        flags |= SDL_FULLSCREEN;
    }

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

    sprintf(temp, "%s V%s", PACKAGE, VERSION);
    SDL_WM_SetCaption(temp, temp);

    DebugPrintOpenGLVersion();
    InitializeAudio(KPConfig::Instance().TextureName.c_str());
    return InitializeAfterOpen();
}

void KPSdl12UserInterface::MainLoop()
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
                case SDL_VIDEORESIZE:
                    screen = SDL_SetVideoMode(event.resize.w, event.resize.h,
                                              KPConfig::Instance().ColorDepth,
                                              SDL_OPENGL | SDL_RESIZABLE);
                    if ( screen )
                    {
                        Reshape(screen->w, screen->h);
                    }
                    else
                    {
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

void KPSdl12UserInterface::Close()
{
    if ( screen )
    {
        SDL_Quit();
        screen = NULL;
    }
    KPConfig::Instance().WriteToFile();

    exit(0);
}

int KPSdl12UserInterface::GetValue(int what) const
{
    switch (what)
    {
        case KP_WINDOW_WIDTH:
            return (screen != NULL ? screen->w : 0);
        case KP_WINDOW_HEIGHT:
            return (screen != NULL ? screen->h : 0);
    }
    return 0;
}


/////////////////////////////////////////////////////////////////////
// Event Handling
/////////////////////////////////////////////////////////////////////

void KPSdl12UserInterface::SwapBuffers()
{
    SDL_GL_SwapBuffers();
}

/////////////////////////////////////////////////////////////////////
// Audio/Music Interface
/////////////////////////////////////////////////////////////////////

void KPSdl12UserInterface::SetStopMusicCallback()
{
    Mix_HookMusicFinished(stopMusicCallback);
}

void KPSdl12UserInterface::stopMusicCallback()
{
    if (KPSdl12UserInterface::instance != NULL)
    {
        KPSdl12UserInterface::instance->StopMusicCallback();
    }
}

#endif //#ifdef HAVE_SDL
