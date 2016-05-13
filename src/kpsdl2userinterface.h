/*
    kpsdl2userinterface.h

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

#ifndef KPSDL2USERINTERFACE_H
#define KPSDL2USERINTERFACE_H

#include "stdafx.h"
#ifdef HAVE_SDL2
#include <SDL_opengl.h>

#include "kpsdluserinterface.h"

class KPnode;

class KPSdl2UserInterface : public KPSdlUserInterface
{
public:
    // public interface
    KPSdl2UserInterface(KPnode &, KPConfigPtr);
    KPSdl2UserInterface() = delete;
    KPSdl2UserInterface(const KPSdl2UserInterface &) = delete;
    KPSdl2UserInterface & operator=(const KPSdl2UserInterface &) = delete;

    void OpenWindow(int argc, char **argv) override;
    void SetWindowMode(bool FullScreen) const override;
    void SetWindowSize(int width, int height) const override;
    void MainLoop() override;
    void Close() override;
    int GetValue(int what) const override;
    bool IsWindowResolutionSupported(int width, int height) const override;
    bool CanToggleFullScreen() const override;
    bool CanChangeWindowSize() const override;

protected:
    // member functions for event handling
    void SwapBuffers() override;
    void DebugPrintOpenGLContextVersion() const;

    SDL_Window   *window;  // SDL Window ID
    SDL_GLContext glContext;
};

#endif //#ifdef HAVE_SDL2
#endif
