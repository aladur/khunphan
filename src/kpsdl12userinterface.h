/*
    kpsdl12userinterface.h

    Automatic solution finder for KhunPhan game
    Copyright (C) 2001-2016  W. Schwotzer

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

#ifndef KPSDL12USERINTERFACE_H
#define KPSDL12USERINTERFACE_H

#include "stdafx.h"
#ifdef HAVE_SDL

#include "kpsdluserinterface.h"

class KPnode;
class KPConfig;

class KPSdl12UserInterface : public KPSdlUserInterface
{
public:
    // public interface
    KPSdl12UserInterface(KPnode &, KPConfig &);

    void OpenWindow(int argc, char **argv);
    void SetWindowMode(bool FullScreen) const;
    void SetWindowSize(int width, int height) const;
    void MainLoop();
    void Close();
    int  GetValue(int what) const;
    bool IsWindowResolutionSupported(int width, int height) const;

protected:
    // member functions for event handling
    void SwapBuffers();
    void SetStopMusicCallback();
    static void stopMusicCallback();

    SDL_Surface  *screen;  // SDL Screen ID
};

#endif //#ifdef HAVE_SDL
#endif
