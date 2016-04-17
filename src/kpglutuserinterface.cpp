/*
    kpglutuserinterface.cpp

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

#include "stdafx.h"
#include <ostream>
#include <stdexcept>
#include "kpscore.h"
#include "blogger.h"

/* If either GLUT or freeglut is available use it */
/* otherwise check for OpenGLUT                   */
#ifdef HAVE_LIBGLUT
    #include <GL/glut.h>
    #ifdef FREEGLUT
        #include <GL/freeglut.h>
    #endif
#else
    #ifdef HAVE_LIBOPENGLUT
        #include <GL/openglut.h>
    #endif
#endif

#if defined(HAVE_LIBOPENGLUT) || defined(HAVE_LIBGLUT)

#include "kpglutuserinterface.h"
#include "kpconfig.h"


KPGlutUserInterface::KPGlutUserInterface(KPnode &rootNode, KPConfig &Config) :
    KPUIBase(Config),
    windowID(0)
{
    Initialize(rootNode);
}

KPGlutUserInterface::~KPGlutUserInterface()
{
    if (windowID > 0)
    {
        glutDestroyWindow(windowID);
        windowID = 0;
    }
}

/////////////////////////////////////////////////////////////////////
// Public Interface
/////////////////////////////////////////////////////////////////////

void KPGlutUserInterface::InitializeEvents()
{
    glutIgnoreKeyRepeat(1);

    glutMouseFunc(KPGlutUserInterface::MouseClickEvent);
    glutKeyboardFunc(KPGlutUserInterface::KeyPressedEvent);

    glutIdleFunc(KPGlutUserInterface::IdleEvent);
    glutDisplayFunc(KPGlutUserInterface::DisplayEvent);
    glutReshapeFunc(KPGlutUserInterface::ReshapeEvent);
}

bool KPGlutUserInterface::CanToggleFullScreen() const
{
#if defined(FREEGLUT) || defined(OPENGLUT)
    return true;
#else
    return false;
#endif
}

#if defined(FREEGLUT) || defined(OPENGLUT)
void KPGlutUserInterface::SetWindowMode(bool FullScreen) const
{
    if (windowID == 0 || !CanToggleFullScreen())
    {
        return;
    }

    if (FullScreen)
    {
        std::stringstream modeString;

        modeString
                << config.ScreenXResolution << 'x'
                << ((config.ScreenXResolution * 3) / 4) << ':'
                << config.ColorDepth;

        glutGameModeString(modeString.str().c_str());

        if (!glutGameModeGet(GLUT_GAME_MODE_POSSIBLE))
        {
            std::cout << "  No Game Mode possible!" << std::endl << std::endl;
        }

        glutFullScreen();
    }
    else
    {
        glutReshapeWindow(config.ScreenXResolution,
                          (config.ScreenXResolution * 3) / 4);
        glutPositionWindow(1, 0);
        glutPositionWindow(0, 0);
    }
}
#else
void KPGlutUserInterface::SetWindowMode(bool /* FullScreen */) const
{
    // Function not supported
}
#endif

bool KPGlutUserInterface::CanChangeWindowSize() const
{
#if defined(FREEGLUT) || defined(OPENGLUT)
    return true;
#else
    return false;
#endif
}

void KPGlutUserInterface::SetWindowSize(int width, int height) const
{
    glutReshapeWindow(width, height);
}

void KPGlutUserInterface::OpenWindow(int argc, char **argv)
{
    glutInit(&argc, argv);
    BLogger::Log("GLUT UserInterface initialization");
#ifdef GLUT_API_VERSION
    BLogger::Log("GLUT API Version ", GLUT_API_VERSION);
#endif
#ifdef GLUT_XLIB_IMPLEMENTATION
    BLogger::Log("GLUT XLIB Implementation #", GLUT_XLIB_IMPLEMENTATION);
#endif
#ifdef FREEGLUT
    int glutVersion = glutGet(GLUT_VERSION);
    BLogger::Log("FREEGLUT Version ",
                 (glutVersion / 10000) % 100, '.',
                 (glutVersion /   100) % 100, '.',
                 glutVersion          % 100);
#endif
#ifdef OPENGLUT
    auto glutVersion = glutGet(GLUT_VERSION);
    BLogger::Log("OPENGLUT Version ",
                 (glutVersion / 10000) % 100, '.',
                 (glutVersion /   100) % 100, '.',
                 glutVersion          % 100);
#endif

    if (!IsWindowResolutionSupported(config.ScreenXResolution,
                                     (config.ScreenXResolution * 3) / 4))
    {
        config.ScreenXResolution = 640;
    }

    // Open OpenGL Window with GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(config.ScreenXResolution,
                       (config.ScreenXResolution * 3) / 4);
    //glutInitWindowPosition (10, 10);
    windowID = glutCreateWindow(GetWindowTitle().c_str());

    if (config.FullScreen)
    {
        glutFullScreen();
    }

    DebugPrintOpenGLVersion();

    if (windowID <= 0)
    {
        throw std::runtime_error("Error in glutCreateWindow");
    }

    InitializeAfterOpen();
}

void KPGlutUserInterface::MainLoop()
{
    glutMainLoop();
}

void KPGlutUserInterface::RequestForClose()
{
    Close();
}

void KPGlutUserInterface::Close()
{

    config.WriteToFile();

#if defined(FREEGLUT) || defined(OPENGLUT)
    glutLeaveMainLoop();
#else
    // glutMainLoop never returns. For this reason
    // this is the exit point.
    // And yes, there are some memory leaks. They have
    // a constant size independent of the playing time.
    delete this;
    exit(0);
#endif
}

int KPGlutUserInterface::GetValue(int what) const
{
    switch (what)
    {
        case KP_WINDOW_WIDTH:
            return (windowID > 0 ? glutGet(GLUT_WINDOW_WIDTH)  : 0);

        case KP_WINDOW_HEIGHT:
            return (windowID > 0 ? glutGet(GLUT_WINDOW_HEIGHT) : 0);
    }

    return 0;
}

bool KPGlutUserInterface::IsWindowResolutionSupported(
    int width, int height) const
{
    return width <= glutGet(GLUT_SCREEN_WIDTH) &&
           (height <= glutGet(GLUT_SCREEN_HEIGHT));
}

/////////////////////////////////////////////////////////////////////
// Static Event Handling
/////////////////////////////////////////////////////////////////////

void KPGlutUserInterface::MouseClickEvent(int button, int state, int x, int y)
{
    KPGlutUserInterface::instance->MouseClick(button, state, x, y);
}

void KPGlutUserInterface::KeyPressedEvent(unsigned char keyPressed, int x,
        int y)
{
    KPGlutUserInterface::instance->KeyPressed(keyPressed, x, y);
}

void KPGlutUserInterface::DisplayEvent()
{
    KPGlutUserInterface::instance->Display();
}

void KPGlutUserInterface::ReshapeEvent(int width, int height)
{
    KPGlutUserInterface::instance->Reshape(width, height);
}

void KPGlutUserInterface::IdleEvent()
{
    KPGlutUserInterface::instance->Idle();
}

/////////////////////////////////////////////////////////////////////
// Event Handling
/////////////////////////////////////////////////////////////////////

void KPGlutUserInterface::SwapBuffers()
{
    glutSwapBuffers();
}

void KPGlutUserInterface::PostWindowRedisplay()
{
    glutPostWindowRedisplay(windowID);
}

void KPGlutUserInterface::MouseClick(int button, int state, int x, int y)
{
    tMouseEvent  kpEvent;
    tMouseButton kpButton;
    auto isValid = true;

    switch (button)
    {
        case GLUT_LEFT_BUTTON:
            kpButton = KP_LEFT_MB;
            break;

        case GLUT_MIDDLE_BUTTON:
            kpButton = KP_MIDDLE_MB;
            break;

        case GLUT_RIGHT_BUTTON:
            kpButton = KP_RIGHT_MB;
            break;

        default:
            isValid = false;
    }

    switch (state)
    {
        case GLUT_DOWN:
            kpEvent = KP_BUTTON_PRESS;
            break;

        case GLUT_UP:
            kpEvent = KP_BUTTON_RELEASE;
            break;

        default:
            isValid = false;
    }

    if (isValid)
    {
        pState->MouseClick(this, kpButton, kpEvent, x, y);
    }
}
#endif

