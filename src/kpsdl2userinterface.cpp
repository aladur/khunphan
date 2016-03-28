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


KPSdl2UserInterface::KPSdl2UserInterface(KPnode &rootNode, KPConfig &Config) :
    KPSdlUserInterface(Config),
    window(NULL), glContext(NULL)
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
    if (window != NULL && CanToggleFullScreen())
    {
        SDL_SetWindowFullscreen(window,
                            isfullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0 );
    }
}

void KPSdl2UserInterface::SetWindowSize(int width, int height) const
{
    if (window != NULL && CanChangeWindowSize())
    {
        int oldWidth, oldHeight;

        SDL_GetWindowSize(window, &oldWidth, &oldHeight);
        SDL_SetWindowSize(window, width, height);

        // Try to reposition the new window centered over the old window
        // Ignore if this is not possible.
        SDL_Rect rect;
        int index, result, x, y, oldX, oldY, diff;

        index = SDL_GetWindowDisplayIndex(window);
        if (index < 0)
        {
            LOG2("*** SDL_GetWindowDisplayIndex error: ", SDL_GetError());
            return;
        }

        result = SDL_GetDisplayBounds(index, &rect);
        if (result < 0)
        {
            LOG2("*** SDL_GetDisplayBounds error: ", SDL_GetError());
            return;
        }

        SDL_GetWindowPosition(window, &oldX, &oldY);

        x = oldX - (width - oldWidth) / 2;
        y = oldY - (height - oldHeight) / 2;

        if (x < rect.x)
        {
            x = rect.x;
        }
        diff = x + width - rect.x - rect.w;
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
    int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    SDL_version compiled;
    SDL_version linked;
    const SDL_version *pVersion;
    std::stringstream message;

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
    LOG2("SDL Revision: ", SDL_GetRevision());
    pVersion = Mix_Linked_Version();
    LOG6("SDL_mixer Linked version: ",
         static_cast<unsigned int>(pVersion->major), '.',
         static_cast<unsigned int>(pVersion->minor), '.',
         static_cast<unsigned int>(pVersion->patch));
    LOG6("SDL_mixer Header version: ",
         MIX_MAJOR_VERSION, '.', MIX_MINOR_VERSION, '.', MIX_PATCHLEVEL);

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
                                     (config.ScreenXResolution*3)/4))
    {
        config.ScreenXResolution = 640;
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
        message << "Error in SDL_CreateWindow: " << SDL_GetError();
        SDL_Quit();
        throw std::runtime_error(message.str());
    }

    // We intentionally do not use the SDL renderer. It only supports 2D
    // and on Windows would use DirectX and not the OpenGL backend.
    // Instead the OpenGL renderer is used together with ligGLEW.
    glContext = SDL_GL_CreateContext(window);
    if (glContext == NULL)
    {
        message << "Error in SDL_GL_CreateContext: " << SDL_GetError();
        SDL_Quit();
        throw std::runtime_error(message.str());
    }
    // Do updates synchronized with VSync
    SDL_GL_SetSwapInterval(1);

    GLenum glewReturn = glewInit();
    if (glewReturn != GLEW_OK)
    {
        message << "Error in glewInit: " << glewGetErrorString(glewReturn);
        SDL_Quit();
        throw std::runtime_error(message.str());
    }
    LOG2("GLEW version: ", glewGetString(GLEW_VERSION));

    SDL_DisplayMode mode;

    SDL_GetWindowDisplayMode(window, &mode);
    LOG2("SDL pixel format: ", SDL_GetPixelFormatName(mode.format));
    LOG3("SDL refresh rate: ", mode.refresh_rate, " Hz");

    DebugPrintOpenGLVersion();
    DebugPrintOpenGLContextVersion();
    InitializeAudio(config.TextureName.c_str());
    InitializeAfterOpen();
}

void KPSdl2UserInterface::DebugPrintOpenGLContextVersion() const
{
    size_t index = 0;
    int major = 0;
    int minor = 0;
    int profile = 0;
    static const int profile2Index[3] = {
        SDL_GL_CONTEXT_PROFILE_CORE, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY,
        SDL_GL_CONTEXT_PROFILE_ES
    };
    static const char *index2String[4] = {
        "SDL_GL_CONTEXT_PROFILE_CORE", "SDL_GL_CONTEXT_PROFILE_COMPATIBILITY",
        "SDL_GL_CONTEXT_PROFILE_ES", "<unknown>"
    };

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

    LOG4("OpenGL context version: ", major, ".", minor);
    LOG2("OpenGL context profile: ", index2String[index]);
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
    if (window != NULL)
    {
        SDL_GL_DeleteContext(glContext);
        glContext = NULL;
        SDL_DestroyWindow(window);
        window = NULL;
        SDL_Quit();
    }
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

bool KPSdl2UserInterface::IsWindowResolutionSupported(
                          int width, int height) const
{
    int index, result;
    SDL_Rect rect;

    index = SDL_GetWindowDisplayIndex(window);
    if (index < 0)
    {
        LOG2("*** SDL_GetWindowDisplayIndex error: ", SDL_GetError());
        return false;
    }

    result = SDL_GetDisplayBounds(index, &rect);
    if (result < 0)
    {
        LOG2("*** SDL_GetDisplayBounds error: ", SDL_GetError());
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
    if (KPSdl2UserInterface::instance != NULL)
    {
        KPSdl2UserInterface::instance->StopMusicCallback();
    }
}

#endif //#ifdef HAVE_SDL2

