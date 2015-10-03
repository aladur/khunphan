/*
    kpuibase.cpp

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
#ifdef TIME_WITH_SYS_TIME
#include <sys/time.h>
#include <time.h>
#else
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#else
#include <time.h>
#endif
#endif
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h> // needed for GLU version
#include "kpuibase.h"
#include "kpstatefactory.h"
#include "kpboardGL.h"
#include "camera.h"
#include "light.h"
#include "kpconfig.h"
#include "kpstate.h"
#include "kpstatistics.h"

// Uncomment the following line to
// compile for a specific light test version
// #define DEBUG_LIGHT_TEST


KPUIBase *KPUIBase::instance = NULL;

KPUIBase::KPUIBase() : proot(NULL), pBoardView(NULL), pCamera(NULL),
    pLight(NULL),
    pMenu(NULL),  pStatistics(NULL), pState(NULL),
    lastFrameTimestamp(0), oldTime(0), frameCount(0)

{
    instance = this;
}

KPUIBase::~KPUIBase()
{
    delete pBoardView;
    pBoardView  = NULL;
    delete pCamera;
    pCamera     = NULL;
    delete pLight;
    pLight      = NULL;
    delete pMenu;
    pMenu       = NULL;
    delete pState;
    pState      = NULL;
    delete pStatistics;
    pStatistics = NULL;
    instance = NULL;
}

/////////////////////////////////////////////////////////////////////
// Public Interface
/////////////////////////////////////////////////////////////////////

bool KPUIBase::IsInitialized()
{
    return instance != NULL;
}

bool KPUIBase::InitializeAfterOpen()
{
    KPConfig &config = KPConfig::Instance();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    glShadeModel(GL_SMOOTH);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glClearColor(0,0,0,0);
    glClearDepth(1.0);
    glDepthFunc(GL_LESS);

    pStatistics = new KPStatistics();

    pBoardView = new KPboardView();
    if (!pBoardView->Initialize(config.TextureName.c_str(),
                                config.TextureSize,
                                config.Nearest))
    {
        return false;
    }

    pLight = new Light(config.AmbientLight,
                       config.LightSources,
                       config.Reflections);

    pCamera = new Camera();

    pMenu = new KPmenu();

    LOG1("Menu initialization");

    if (!pMenu->Initialize(config.TextureName,
                           config.MenuTextureSize,
                           config.Nearest,
                           config.Language))
    {
        return false;
    }

#ifdef DEBUG_LIGHT_TEST
    ChangeState(KPState_LightTest);
#else
    ChangeState(KPState_StartUp);
#endif

    InitializeEvents();

    return true;
}

std::string KPUIBase::GetWindowTitle() const
{
    std::stringstream title;

    title << PACKAGE << " V" << VERSION;

    return title.str();
}

void KPUIBase::UpdateDataModel(KPnode *pRoot)
{
    pBoardView->SetSolveTree(pRoot);
}

std::string KPUIBase::GetOpenGLVendor() const
{
    return reinterpret_cast<const char *>(glGetString(GL_VENDOR));
}

std::string KPUIBase::GetOpenGLRenderer() const
{
    return reinterpret_cast<const char *>(glGetString(GL_RENDERER));
}

std::string KPUIBase::GetOpenGLVersion() const
{
    return reinterpret_cast<const char *>(glGetString(GL_VERSION));
}

void KPUIBase::DebugPrintOpenGLVersion() const
{
    LOG2("OpenGL Vendor:   ", GetOpenGLVendor());
    LOG2("OpenGL Renderer: ", GetOpenGLRenderer());
    LOG2("OpenGL Version:  ", GetOpenGLVersion());
#ifdef GLU_VERSION_1_3
    LOG1("GLU Header Version 1.3");
#else
#ifdef GLU_VERSION_1_2
    LOG1("GLU Header Version 1.2");
#else
#ifdef GLU_VERSION_1_1
    LOG1("GLU Header Version 1.1");
#else
    LOG1("GLU Header Version 1.0");
#endif
#endif
#endif
#ifdef GLU_VERSION_1_1
    LOG2("GLU Linked Version ", gluGetString(GLU_VERSION));
#endif
}

/////////////////////////////////////////////////////////////////////
// Event Handling
/////////////////////////////////////////////////////////////////////

void KPUIBase::Display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (KPConfig::Instance().DisplayFPS)
    {
        DisplayFPS();
    }

    pState->Draw(this);  // Drawing is delegated to KPstate
    glFinish();

    SwapBuffers();
}

void KPUIBase::Reshape(int x, int y)
{
    glViewport(0, 0, x, y);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GetCamera().SetAspectRatio(x / static_cast<float>(y));
}

void KPUIBase::Idle()
{
    if (!pState)  // Is Closing in progress?
    {
        return;
    }

    if (lastFrameTimestamp == 0)
    {
        // first time initialization
        lastFrameTimestamp = fpsTime.GetTimeMsl();
        return;
    }

    unsigned long frameTimestamp = fpsTime.GetTimeMsl();
    unsigned long factor = frameTimestamp - lastFrameTimestamp;

    if (factor >= 10)
    {

        pState->Update(this, factor / 10);  // Updating is delegated to KPstate

        lastFrameTimestamp = frameTimestamp;

        PostWindowRedisplay();
    }
}

void KPUIBase::DisplayFPS()
{
    unsigned long t = fpsTime.GetTimeMsl();

    frameCount++;
    if (t - oldTime > 1000)  // update every second
    {
        pMenu->UpdateFPS(frameCount,
                         static_cast<float>(t - oldTime) / frameCount);
        oldTime    = t;
        frameCount = 0;
    }
}

void KPUIBase::Visible (int /* vis */) const
{
    // unfinished
//  if (vis == GLUT_VISIBLE)
//    ;
}

void KPUIBase::MouseMotion( int x, int y )
{
    pState->MouseMotion(this, x, y);
}

void KPUIBase::KeyPressed( unsigned char keyPressed, int x, int y )
{
    pState->KeyPressed(this, keyPressed, x, y);
}

void KPUIBase::KeyReleased( unsigned char keyReleased, int x, int y )
{
    pState->KeyReleased(this, keyReleased, x, y);
}

/////////////////////////////////////////////////////////////////////
// Implementation of State interface
/////////////////////////////////////////////////////////////////////

void KPUIBase::ChangeState( int stateID )
{
    KPstate *pOldState = pState;
    pState = KPstateFactory::Instance().CreateState( stateID );
    if (pState)
    {
        pState->Initialize(this, pOldState);
    }
    delete pOldState;
    if (pState == NULL)
    {
        delete pBoardView;
        pBoardView  = NULL;
        delete pCamera;
        pCamera     = NULL;
        delete pLight;
        pLight      = NULL;
        delete pMenu;
        pMenu       = NULL;
        delete pState;
        pState      = NULL;
        delete pStatistics;
        pStatistics = NULL;
        Close();
    }
}

KPboardView     &KPUIBase::GetBoardView()
{
    return *pBoardView;
}
Camera          &KPUIBase::GetCamera()
{
    return *pCamera;
}
Light           &KPUIBase::GetLight()
{
    return *pLight;
}
KPmenu          &KPUIBase::GetMenu()
{
    return *pMenu;
}
KPStatistics    &KPUIBase::GetStatistics()
{
    return *pStatistics;
}
KPUIBase        &KPUIBase::GetUserInterface()
{
    return *instance;
}

/////////////////////////////////////////////////////////////////////
// Implementation of Audio interface
/////////////////////////////////////////////////////////////////////

bool KPUIBase::InitializeAudio(const char *, bool)
{
    // default implementation: No Audio support, no error
    return true;
}

void KPUIBase::PlayAudio(int /* soundId */) const
{
}

void KPUIBase::SetSoundVolume(int /* volume */) const
{
}

void KPUIBase::SetMusicVolume(int /* volume */) const
{
}

void KPUIBase::PlayMusic(bool /* On */, bool /* resetPos */)
{
}

void KPUIBase::StopMusicCallback()
{
}
