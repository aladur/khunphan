/*
    kpsdl2userinterface.cpp

    Automatic solution finder for KhunPhan game
    Copyright (C) 2015-2016  W. Schwotzer

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
#ifdef HAVE_SDL2

#include <stdexcept>
#include <GL/glew.h>
#include "kpsdl2userinterface.h"
#include "kpconfig.h"
#include "blogger.h"


KPSdl2UserInterface::KPSdl2UserInterface(KPnode &rootNode, KPConfig &Config) :
    KPSdlUserInterface(Config),
    window(nullptr), glContext(nullptr)
{
    Initialize(rootNode);
}

/////////////////////////////////////////////////////////////////////
// Public Interface
/////////////////////////////////////////////////////////////////////

bool KPSdl2UserInterface::CanChangeWindowSize() const
{
    return true;
}

bool KPSdl2UserInterface::CanToggleFullScreen() const
{
    return true;
}

void KPSdl2UserInterface::SetWindowMode(bool isfullscreen) const
{
    if (window != nullptr && CanToggleFullScreen())
    {
        auto flags = isfullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0;
        SDL_SetWindowFullscreen(window, flags);
    }
}

void KPSdl2UserInterface::SetWindowSize(int width, int height) const
{
    if (window != nullptr && CanChangeWindowSize())
    {
        int oldWidth, oldHeight;

        SDL_GetWindowSize(window, &oldWidth, &oldHeight);
        SDL_SetWindowSize(window, width, height);

        // Try to reposition the new window centered over the old window
        // Ignore if this is not possible.
        SDL_Rect rect;
        int oldX, oldY;

        auto index = SDL_GetWindowDisplayIndex(window);

        if (index < 0)
        {
            BLogger::Log("*** SDL_GetWindowDisplayIndex error: ",
                         SDL_GetError());
            return;
        }

        auto result = SDL_GetDisplayBounds(index, &rect);

        if (result < 0)
        {
            BLogger::Log("*** SDL_GetDisplayBounds error: ", SDL_GetError());
            return;
        }

        SDL_GetWindowPosition(window, &oldX, &oldY);

        auto x = oldX - (width - oldWidth) / 2;
        auto y = oldY - (height - oldHeight) / 2;

        if (x < rect.x)
        {
            x = rect.x;
        }

        auto diff = x + width - rect.x - rect.w;

        if (diff > 0)
        {
            x -= diff;
        }

        if (y < rect.y)
        {
            y = rect.y;
        }

        diff = y + height - rect.y - rect.h;

        if (diff > 0)
        {
            y -= diff;
        }

        SDL_SetWindowPosition(window, x, y);
    }
}

void KPSdl2UserInterface::OpenWindow(int /* argc */ , char ** /* argv */)
{
    auto flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    SDL_version compiled;
    SDL_version linked;
    std::stringstream message;

    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);

    BLogger::Log("SDL UserInterface initialization");
    BLogger::Log("SDL linked version: ",
                 static_cast<unsigned int>(linked.major), '.',
                 static_cast<unsigned int>(linked.minor), '.',
                 static_cast<unsigned int>(linked.patch));
    BLogger::Log("SDL compiled version: ",
                 static_cast<unsigned int>(compiled.major), '.',
                 static_cast<unsigned int>(compiled.minor), '.',
                 static_cast<unsigned int>(compiled.patch));
    BLogger::Log("SDL Header version: ",
                 SDL_MAJOR_VERSION, '.', SDL_MINOR_VERSION, '.',
                 SDL_PATCHLEVEL);
    BLogger::Log("SDL Revision: ", SDL_GetRevision());
    auto pVersion = Mix_Linked_Version();
    BLogger::Log("SDL_mixer Linked version: ",
                 static_cast<unsigned int>(pVersion->major), '.',
                 static_cast<unsigned int>(pVersion->minor), '.',
                 static_cast<unsigned int>(pVersion->patch));
    BLogger::Log("SDL_mixer Header version: ",
                 MIX_MAJOR_VERSION, '.', MIX_MINOR_VERSION, '.',
                 MIX_PATCHLEVEL);

    // Set OpenGL's context to 2.1 subset functionality profile.
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

    // Open OpenGL Window with SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        message << "Error in SDL_Init: " << SDL_GetError();
        SDL_Quit();
        throw std::runtime_error(message.str());
    }

    if (config.FullScreen)
    {
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }

    if (!IsWindowResolutionSupported(config.ScreenXResolution,
                                     (config.ScreenXResolution * 3) / 4))
    {
        config.ScreenXResolution = 640;
    }

    window = SDL_CreateWindow(
                 GetWindowTitle().c_str(),
                 SDL_WINDOWPOS_UNDEFINED,
                 SDL_WINDOWPOS_UNDEFINED,
                 config.ScreenXResolution,
                 (config.ScreenXResolution * 3) / 4,
                 flags);

    if (window == nullptr)
    {
        message << "Error in SDL_CreateWindow: " << SDL_GetError();
        SDL_Quit();
        throw std::runtime_error(message.str());
    }

    // We intentionally do not use the SDL renderer. It only supports 2D
    // and on Windows would use DirectX and not the OpenGL backend.
    // Instead the OpenGL renderer is used together with ligGLEW.
    glContext = SDL_GL_CreateContext(window);

    if (glContext == nullptr)
    {
        message << "Error in SDL_GL_CreateContext: " << SDL_GetError();
        SDL_Quit();
        throw std::runtime_error(message.str());
    }

    // Do updates synchronized with VSync
    SDL_GL_SetSwapInterval(1);

    auto glewReturn = glewInit();

    if (glewReturn != GLEW_OK)
    {
        message << "Error in glewInit: " << glewGetErrorString(glewReturn);
        SDL_Quit();
        throw std::runtime_error(message.str());
    }

    BLogger::Log("GLEW version: ", glewGetString(GLEW_VERSION));

    SDL_DisplayMode mode;

    SDL_GetWindowDisplayMode(window, &mode);
    BLogger::Log("SDL pixel format: ", SDL_GetPixelFormatName(mode.format));
    BLogger::Log("SDL refresh rate: ", mode.refresh_rate, " Hz");

    DebugPrintOpenGLVersion();
    DebugPrintOpenGLContextVersion();
    InitializeAudio(config.TextureName.c_str());
    InitializeAfterOpen();
}

void KPSdl2UserInterface::DebugPrintOpenGLContextVersion() const
{
    int major = 0;
    int minor = 0;
    int profile = 0;
    static const int profile2Index[3] =
    {
        SDL_GL_CONTEXT_PROFILE_CORE, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY,
        SDL_GL_CONTEXT_PROFILE_ES
    };
    static const char *index2String[4] =
    {
        "SDL_GL_CONTEXT_PROFILE_CORE", "SDL_GL_CONTEXT_PROFILE_COMPATIBILITY",
        "SDL_GL_CONTEXT_PROFILE_ES", "<unknown>"
    };
    decltype(sizeof(profile2Index)) index = 0;

    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &profile);

    for (; index < (sizeof(profile2Index) / sizeof(profile2Index[0])); ++index)
    {
        if (profile2Index[index] == profile)
        {
            break;
        }
    }

    BLogger::Log("OpenGL context version: ", major, ".", minor);
    BLogger::Log("OpenGL context profile: ", index2String[index]);
}

void KPSdl2UserInterface::MainLoop()
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
                case SDL_WINDOWEVENT:
                    switch (event.window.event)
                    {
                        case SDL_WINDOWEVENT_SIZE_CHANGED:
                            Reshape(event.window.data1, event.window.data2);
                            break;
                    }

                    break;

                case SDL_USEREVENT:
                    if (event.user.code == REQUEST_FOR_CLOSE)
                    {
                        done = true;
                    }

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

void KPSdl2UserInterface::Close()
{
    if (window != nullptr)
    {
        SDL_GL_DeleteContext(glContext);
        glContext = nullptr;
        SDL_DestroyWindow(window);
        window = nullptr;
        SDL_Quit();
    }
}

int KPSdl2UserInterface::GetValue(int what) const
{
    if (window != nullptr)
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

bool KPSdl2UserInterface::IsWindowResolutionSupported(
    int width, int height) const
{
    int index;
    SDL_Rect rect;

    if (window == nullptr)
    {
        // If there is no window opened yet we just can make a guess.
        index = 0;
    }
    else
    {
        index = SDL_GetWindowDisplayIndex(window);
    }

    if (index < 0)
    {
        BLogger::Log("*** SDL_GetWindowDisplayIndex error: ", SDL_GetError());
        return false;
    }

    auto result = SDL_GetDisplayBounds(index, &rect);

    if (result < 0)
    {
        BLogger::Log("*** SDL_GetDisplayBounds error: ", SDL_GetError());
        return false;
    }

    return width <= rect.w && (height <= rect.h);
}

/////////////////////////////////////////////////////////////////////
// Event Handling
/////////////////////////////////////////////////////////////////////

void KPSdl2UserInterface::SwapBuffers()
{
    SDL_GL_SwapWindow(window);
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
    if (KPSdl2UserInterface::instance != nullptr)
    {
        KPSdl2UserInterface::instance->StopMusicCallback();
    }
}

#endif //#ifdef HAVE_SDL2

