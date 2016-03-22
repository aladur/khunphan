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

#include "kpsdluserinterface.h"

class KPnode;
class KPConfig;

class KPSdl2UserInterface : public KPSdlUserInterface
{
public:
    // public interface
    KPSdl2UserInterface(KPnode &, KPConfig &);

    void OpenWindow(int argc, char **argv);
    void SetWindowMode(bool FullScreen) const;
    void SetWindowSize(int width, int height) const;
    void MainLoop();
    void Close();
    int  GetValue(int what) const;
    bool CanToggleFullScreen() const;
    bool CanChangeWindowSize() const;

protected:
    // member functions for event handling
    void SwapBuffers();
    void SetStopMusicCallback();


    static void stopMusicCallback();

    SDL_Window   *window;  // SDL Window ID
    SDL_Renderer *renderer;
};

#endif //#ifdef HAVE_SDL2
#endif
