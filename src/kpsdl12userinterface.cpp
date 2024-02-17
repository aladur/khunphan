/*
    kpsdl12userinterface.cpp

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
#ifdef HAVE_SDL

#include <stdexcept>
#include "kpsdl12userinterface.h"
#include "kpconfig.h"
#include "blogger.h"


KPSdl12UserInterface::KPSdl12UserInterface(KPnode &rootNode,
        KPConfigPtr PConfig) :
    KPSdlUserInterface(PConfig), screen(nullptr)
{
    Initialize(rootNode);
}

/////////////////////////////////////////////////////////////////////
// Public Interface
/////////////////////////////////////////////////////////////////////

void KPSdl12UserInterface::SetWindowMode(bool /* FullScreen */) const
{
    // Toggle Full-screen and window mode is unreliable in SDL V1.2.
    // It has been removed.
}

void KPSdl12UserInterface::SetWindowSize(int, int) const
{
    // Function not supported
}

void KPSdl12UserInterface::OpenWindow(int /* argc */ , char ** /* argv */)
{
    auto flags = SDL_OPENGL | SDL_RESIZABLE;

    BLogger::Log("SDL UserInterface initialization");
    auto pVersion = SDL_Linked_Version();
    BLogger::Log("SDL Linked version: ",
                 static_cast<unsigned int>(pVersion->major), '.',
                 static_cast<unsigned int>(pVersion->minor), '.',
                 static_cast<unsigned int>(pVersion->patch));
    BLogger::Log("SDL Header version: ",
                 SDL_MAJOR_VERSION, '.', SDL_MINOR_VERSION, '.',
                 SDL_PATCHLEVEL);
    pVersion = Mix_Linked_Version();
    BLogger::Log("SDL_mixer Linked version: ",
                 static_cast<unsigned int>(pVersion->major), '.',
                 static_cast<unsigned int>(pVersion->minor), '.',
                 static_cast<unsigned int>(pVersion->patch));
    BLogger::Log("SDL_mixer Header version: ",
                 MIX_MAJOR_VERSION, '.', MIX_MINOR_VERSION, '.',
                 MIX_PATCHLEVEL);

    // Open OpenGL Window with SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_NOPARACHUTE) < 0)
    {
        std::stringstream message;

        message << "Error in SDL_Init: " << SDL_GetError();
        throw std::runtime_error(message.str());
    }

    // Full-screen mode is not supported. Disable full-screen flag.
    config->FullScreen = false;

    if (!IsWindowResolutionSupported(config->ScreenXResolution,
                                     (config->ScreenXResolution * 3) / 4))
    {
        config->ScreenXResolution = 640;
    }

    screen = SDL_SetVideoMode(
                 config->ScreenXResolution,
                 (config->ScreenXResolution * 3) / 4,
                 config->ColorDepth,
                 flags);

    if (screen == nullptr)
    {
        std::stringstream message;

        message << "Error in SDL_SetVideoMode: " << SDL_GetError();
        SDL_Quit();
        throw std::runtime_error(message.str());
    }

    SDL_WM_SetCaption(GetWindowTitle().c_str(), GetWindowTitle().c_str());

    DebugPrintOpenGLVersion();
    InitializeAudio(config->TextureName);
    InitializeAfterOpen();
}

void KPSdl12UserInterface::MainLoop()
{
    // This is the event loop
    auto done = false;
    unsigned char key;

    while (! done)
    {
        SDL_Event event;

        memset(&event, 0, sizeof(event));

        Idle();

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_VIDEORESIZE:
                    screen = SDL_SetVideoMode(event.resize.w, event.resize.h,
                                              config->ColorDepth,
                                              SDL_OPENGL | SDL_RESIZABLE);

                    if (screen)
                    {
                        Reshape(screen->w, screen->h);
                    }
                    else
                    {
                        std::stringstream message;

                        message << "Error in SDL_SetVideoMode: "
                                << SDL_GetError();
                        throw std::runtime_error(message.str());
                    }

                    break;

                case SDL_USEREVENT:
                    if (event.user.code == REQUEST_FOR_CLOSE)
                    {
                        done = true;
                    }

                    break;

                case SDL_QUIT:
                    done = true;
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
            } // switch
        } // event loop

        Display();
        PostWindowRedisplay();

        SDL_Delay(10);
    } // main loop
}

void KPSdl12UserInterface::Close()
{
    if (screen)
    {
        SDL_Quit();
        screen = nullptr;
    }
}

int KPSdl12UserInterface::Get(WindowProperty what) const
{
    if (screen != nullptr)
    {
        switch (what)
        {
            case WindowProperty::Width:
                return screen->w;

            case WindowProperty::Height:
                return screen->h;
        }
    }

    return 0;
}

bool KPSdl12UserInterface::IsWindowResolutionSupported(int, int) const
{
    // How to do this? SDL_VideoModeOK doesn't work here.
    // Maybe the right time to switch to SDL2?
    return true;
}

/////////////////////////////////////////////////////////////////////
// Event Handling
/////////////////////////////////////////////////////////////////////

void KPSdl12UserInterface::SwapBuffers()
{
    SDL_GL_SwapBuffers();
}

#endif //#ifdef HAVE_SDL
