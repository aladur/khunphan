/*
    kpuibase.cpp

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

#ifdef LINUX
#include <sys/time.h>
#endif
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h> // needed for GLU version
#include "misc1.h"
#include "kpuibase.h"
#include "kpstatefactory.h"
#include "kpboardGL.h"
#include "Kamera.h"
#include "light.h"
#include "kpconfig.h"
#include "kpstate.h"
#include "kpstatistics.h"
#include "btime.h"

KPUIBase *KPUIBase::instance = NULL;

KPUIBase::KPUIBase() : pBoardView(NULL), pCamera(NULL), pLight(NULL),
                       pMenu(NULL),  pStatistics(NULL), pState(NULL),
                       lastFrameTimestamp(0)
                       
{
  instance = this;
  pTime    = new BTime;
}

KPUIBase::~KPUIBase()
{
  delete pBoardView;  pBoardView  = NULL;
  delete pCamera;     pCamera     = NULL;
  delete pLight;      pLight      = NULL;
  delete pMenu;       pMenu       = NULL;
  delete pState;      pState      = NULL;
  delete pStatistics; pStatistics = NULL;
  delete pTime;       pTime       = NULL;
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
  if (!pBoardView->Initialize(KPConfig::Instance().TextureName,
                              KPConfig::Instance().TextureSize,
                              KPConfig::Instance().Nearest))
    return false;

  pLight = new Light(KPConfig::Instance().AmbientLight,
                     KPConfig::Instance().LightSources,
                     KPConfig::Instance().Reflections);

  pCamera = new Kamera();

  pMenu = new KPmenu();

  DEBUGPRINT("Menu initialization\n");

  if (!pMenu->Initialize(KPConfig::Instance().TextureName,
                         KPConfig::Instance().MenuTextureSize,
                         KPConfig::Instance().Nearest,
                         KPConfig::Instance().Language))
  return false;

  ChangeState(KPState_StartUp);

  InitializeEvents();

  return true;
}

void KPUIBase::UpdateDataModel(KPnode *pRoot)
{
  pBoardView->SetSolveTree(pRoot);
}

const char *KPUIBase::GetOpenGLVendor() const
{
  return (const char *)glGetString(GL_VENDOR);
}

const char *KPUIBase::GetOpenGLRenderer() const
{
  return (const char *)glGetString(GL_RENDERER);
}

const char *KPUIBase::GetOpenGLVersion() const
{
  return (const char *)glGetString(GL_VERSION);
}

void KPUIBase::DebugPrintOpenGLVersion() const
{
  DEBUGPRINT1("OpenGL Vendor:   %s\n", GetOpenGLVendor()  );
  DEBUGPRINT1("OpenGL Renderer: %s\n", GetOpenGLRenderer());
  DEBUGPRINT1("OpenGL Version:  %s\n", GetOpenGLVersion() );
#ifdef GLU_VERSION_1_3
  DEBUGPRINT("GLU Header Version 1.3\n");
#else
  #ifdef GLU_VERSION_1_2
    DEBUGPRINT("GLU Header Version 1.2\n");
  #else
    #ifdef GLU_VERSION_1_1
      DEBUGPRINT("GLU Header Version 1.1\n");
    #else
      DEBUGPRINT("GLU Header Version 1.0\n");
    #endif
  #endif
#endif
#ifdef GLU_VERSION_1_1
  DEBUGPRINT1("GLU Linked Version %s\n", gluGetString(GLU_VERSION));
#endif
}

/////////////////////////////////////////////////////////////////////
// Event Handling
/////////////////////////////////////////////////////////////////////

void KPUIBase::Display()
{
  static double renderTime = 0.0;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  pTime->ResetRelativeTime();

  if (KPConfig::Instance().DisplayFPS)
    DisplayFPS(renderTime);

  pState->Draw(this);  // Drawing is delegated to KPstate
  glFinish();

  renderTime = pTime->GetRelativeTimeUsf() / 1000.0; // Get RenderTime in ms

  SwapBuffers();
}

void KPUIBase::Reshape(int x, int y)
{
  glViewport(0, 0, x, y);
  GetCamera().SetAspekt(x / (GLfloat)y);
}

void KPUIBase::Idle()
{
  if (!pState)  // Is Closing in progress?
    return;

  if (lastFrameTimestamp == 0)
  {
    // first time initialization
    lastFrameTimestamp = pTime->GetTimeMsl();
    return;
  }

  unsigned long frameTimestamp = pTime->GetTimeMsl();
  unsigned long factor = frameTimestamp - lastFrameTimestamp;

  if (factor >= 10) {

    pState->Update(this, factor / 10);  // Updating is delegated to KPstate

    lastFrameTimestamp = frameTimestamp;

    PostWindowRedisplay();
  }
}

void KPUIBase::DisplayFPS(float renderTime)
{
  static unsigned long t0     = 0;
  static unsigned int  Frames = 0;

  unsigned long t = pTime->GetTimeMsl();
  Frames++;
  if (t - t0 > 2000)  // update every 2 seconds
  {
     pMenu->UpdateFPS( Frames / 2, renderTime );
     t0     = t;
     Frames = 0;
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
    pState->Initialize(this, pOldState);
  delete pOldState;
  if (pState == NULL)
  {
    delete pBoardView;  pBoardView  = NULL;
    delete pCamera;     pCamera     = NULL;
    delete pLight;      pLight      = NULL;
    delete pMenu;       pMenu       = NULL;
    delete pState;      pState      = NULL;
    delete pStatistics; pStatistics = NULL;
    delete pTime;       pTime       = NULL;
    Close();
  }
}

KPboardView     &KPUIBase::GetBoardView()     { return *pBoardView;  }
Kamera          &KPUIBase::GetCamera()        { return *pCamera;     }
Light           &KPUIBase::GetLight()         { return *pLight;      }
KPmenu          &KPUIBase::GetMenu()          { return *pMenu;       }
KPStatistics    &KPUIBase::GetStatistics()    { return *pStatistics; }
KPUIBase        &KPUIBase::GetUserInterface() { return *instance;    }

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
