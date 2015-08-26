/*
    kpglutuserinterface.cpp

    Automatic solution finder for KhunPhan game
    Copyright (C) 2001,2002,2003  W. Schwotzer

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

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <GL/glut.h>
#include "kpglutuserinterface.h"

 
KPGlutUserInterface::KPGlutUserInterface() : windowID(0)
{
}

KPGlutUserInterface::~KPGlutUserInterface()
{
}

/////////////////////////////////////////////////////////////////////
// Public Interface
/////////////////////////////////////////////////////////////////////

void KPGlutUserInterface::InitializeEvents()
{
  glutIgnoreKeyRepeat(1);

  glutMouseFunc     (KPGlutUserInterface::MouseClickEvent);
  glutMotionFunc    (KPGlutUserInterface::MouseMotionEvent);
  glutKeyboardFunc  (KPGlutUserInterface::KeyPressedEvent);
  glutKeyboardUpFunc(KPGlutUserInterface::KeyReleasedEvent);

  glutVisibilityFunc(KPGlutUserInterface::VisibleEvent);
  glutIdleFunc      (KPGlutUserInterface::IdleEvent);
  glutDisplayFunc   (KPGlutUserInterface::DisplayEvent);
  glutReshapeFunc   (KPGlutUserInterface::ReshapeEvent);
//  glutTimerFunc     (10, KPGlutUserInterface::TimerEvent, 10); // Timer event every 10 millisec.
}

void KPGlutUserInterface::SetWindowMode(bool /* FullScreen */) const
{
/*
  Sw: unfinished
  if (windowID == 0)
    return;

  if (FullScreen) {
    //sprintf(temp,"%ix%i:%i",KPConfig::Instance().ScreenXResolution,(KPConfig::Instance().ScreenXResolution*3)/4,
    //  KPConfig::Instance().ColorDepth);
    //glutGameModeString(temp);
    //if (!glutGameModeGet(GLUT_GAME_MODE_POSSIBLE))
    //  printf ("  No Game Mode possible!\n\n");
    glutFullScreen();
  } else {
    //sprintf(temp, "%s V%s", PACKAGE, VERSION);
    //windowID = glutCreateWindow (temp);
    glutReshapeWindow(KPConfig::Instance().ScreenXResolution,
                     (KPConfig::Instance().ScreenXResolution*3)/4);
  }
*/
}

bool KPGlutUserInterface::OpenWindow(int argc, char **argv)
{
  char temp[64];

  DEBUGPRINT("GLUT UserInterface initialization\n");
  DEBUGPRINT2("GLUT API Version %d, GLUT XLIB Implementation #%d\n",
	           GLUT_API_VERSION, GLUT_XLIB_IMPLEMENTATION);
  // Open OpenGL Window with GLUT
  glutInit(&argc, argv);
  glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize (KPConfig::Instance().ScreenXResolution,(KPConfig::Instance().ScreenXResolution*3)/4);
  //glutInitWindowPosition (10, 10);
  sprintf(temp, "%s V%s", PACKAGE, VERSION);
  windowID = glutCreateWindow (temp);
  if (KPConfig::Instance().FullScreen)
    glutFullScreen();

  DebugPrintOpenGLVersion();
  if ( windowID <= 0)
    return false;

  return InitializeAfterOpen();
}

void KPGlutUserInterface::MainLoop()
{
    glutMainLoop();
}

void KPGlutUserInterface::Close()
{
  if (windowID > 0)
    glutDestroyWindow(windowID);
  windowID = 0;
  KPConfig::Instance().WriteToFile();
  // The Destruction of the singleton instances
  // is done with the atexit() function
  exit(0);
}

int KPGlutUserInterface::GetValue(int what) const
{
  switch (what)
  {
    case KP_WINDOW_WIDTH: return  (windowID > 0 ? glutGet(GLUT_WINDOW_WIDTH)  : 0);
    case KP_WINDOW_HEIGHT: return (windowID > 0 ? glutGet(GLUT_WINDOW_HEIGHT) : 0);
  }
  return 0;
}

/////////////////////////////////////////////////////////////////////
// Static Event Handling
/////////////////////////////////////////////////////////////////////

void KPGlutUserInterface::MouseClickEvent(int button, int state, int x, int y)
{
  KPGlutUserInterface::instance->MouseClick(button, state, x, y);
}

void KPGlutUserInterface::MouseMotionEvent(int x, int y)
{
  KPGlutUserInterface::instance->MouseMotion(x, y);
}

void KPGlutUserInterface::KeyPressedEvent ( unsigned char keyPressed,  int x, int y )
{
  KPGlutUserInterface::instance->KeyPressed(keyPressed, x, y);
}

void KPGlutUserInterface::KeyReleasedEvent( unsigned char keyReleased, int x, int y )
{
  KPGlutUserInterface::instance->KeyReleased(keyReleased, x, y);
}

void KPGlutUserInterface::VisibleEvent (int vis)
{
  KPGlutUserInterface::instance->Visible(vis);
}

void KPGlutUserInterface::DisplayEvent()
{
  KPGlutUserInterface::instance->Display();
}

void KPGlutUserInterface::ReshapeEvent(int x, int y)
{
  KPGlutUserInterface::instance->Reshape(x, y);
}

void KPGlutUserInterface::IdleEvent()
{
  KPGlutUserInterface::instance->Idle();
}

void KPGlutUserInterface::TimerEvent(int)
{
  //KPGlutUserInterface::instance->Timer(value);
}

/////////////////////////////////////////////////////////////////////
// Event Handling
/////////////////////////////////////////////////////////////////////

void KPGlutUserInterface::SwapBuffers()
{
  glutSwapBuffers();
}

void KPGlutUserInterface::Timer(int)
{
  Idle();
  glutTimerFunc(10, KPGlutUserInterface::TimerEvent, 10);
}

void KPGlutUserInterface::PostWindowRedisplay()
{
  glutPostWindowRedisplay( windowID );
}

void KPGlutUserInterface::MouseClick( int button, int state, int x, int y )
{
  int kpState, kpButton;

  switch (button)
  {
    case GLUT_LEFT_BUTTON:    kpButton = KP_LEFT_MB;   break;
    case GLUT_MIDDLE_BUTTON:  kpButton = KP_MIDDLE_MB; break;
    case GLUT_RIGHT_BUTTON:   kpButton = KP_RIGHT_MB;  break;
    default:                  kpButton = 0;
  }
  switch (state)
  {
    case GLUT_DOWN:         kpState = KP_BUTTON_PRESS;   break;
    case GLUT_UP:           kpState = KP_BUTTON_RELEASE; break;
    default:                kpState = 0;
  }
  pState->MouseClick(this, kpButton, kpState, x, y);
}
