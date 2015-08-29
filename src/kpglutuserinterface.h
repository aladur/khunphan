/*
    kpglutuserinterface.h

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

#ifndef KPGLUTUSERINTERFACE_H
#define KPGLUTUSERINTERFACE_H

#include "misc1.h"

#if defined(HAVE_LIBGLUT) || defined(HAVE_LIBOPENGLUT)

#include "kpuibase.h"
#include "kpconfig.h"
#include "kpstate.h"

class KPGlutUserInterface : public KPUIBase {
public:
  // public interface
  KPGlutUserInterface();
  virtual ~KPGlutUserInterface();

  bool OpenWindow(int argc, char **argv);
  void SetWindowMode(bool FullScreen) const;
  void MainLoop();
  void Close();
  int  GetValue(int what) const;
  bool CanToggleFullScreen() const;

public:
  // static member functions for event handling
  // necessary for event handling
  static void MouseClickEvent(int button, int state, int x, int y);
  static void MouseMotionEvent(int x, int y);
  static void KeyPressedEvent ( unsigned char keyPressed,  int x, int y );
  static void KeyReleasedEvent( unsigned char keyReleased, int x, int y );
  static void IdleEvent();
  static void VisibleEvent (int vis);
  static void DisplayEvent();
  static void ReshapeEvent(int x, int y);
  static void TimerEvent(int value);

  // member functions for event handling
  void Timer(int value);
  void MouseClick(int button, int state, int x, int y);

protected:
  void InitializeEvents();
  void SwapBuffers();
  void PostWindowRedisplay();

  int  windowID;  // ID of GLUT window
};

#endif
#endif
