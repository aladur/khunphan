/*
    kpuibase.cpp

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
#include <GL/gl.h>
#include <GL/glu.h> // needed for GLU version
#include <stdexcept>
#include "kpuibase.h"
#include "kpstatefactory.h"
#include "kpboardGL.h"
#include "camera.h"
#include "light.h"
#include "kpconfig.h"
#include "kpstate.h"
#include "kpstatistics.h"
#include "kpnodes.h"
#include "kpconfig.h"
#include "blogger.h"

// Uncomment the following line to
// compile for a specific light test version
// #define DEBUG_LIGHT_TEST

KPUIBase *KPUIBase::instance = nullptr;

KPUIBase::KPUIBase(KPConfig &Config) :
    pBoardView(nullptr), pCamera(nullptr),
    pLight(nullptr), pMenu(nullptr),
    pNodes(nullptr), pStatistics(nullptr),
    animationTimer(TOTAL_ANIMATIONTIME, false),
    previousStateId(KPState_Invalid), isPause(false),
    pState(nullptr),
    config(Config),
    lastFrameTimestamp(0), oldTime(0), frameCount(0)

{
    // the instance is needed for callback handling.
    // Concequence: Only one UserInterface instance is
    // supported at a time
    if (instance != nullptr)
    {
        throw std::runtime_error(
            "Error: Only one UserInterface instance allowed at a time");
    }
    else
    {
        instance = this;
    }
}

KPUIBase::~KPUIBase()
{
    delete pBoardView;
    pBoardView  = nullptr;
    delete pCamera;
    pCamera     = nullptr;
    delete pLight;
    pLight      = nullptr;
    delete pMenu;
    pMenu       = nullptr;
    delete pNodes;
    pNodes      = nullptr;
    delete pState;
    pState      = nullptr;
    delete pStatistics;
    pStatistics = nullptr;
    instance = nullptr;
}

/////////////////////////////////////////////////////////////////////
// Public Interface
/////////////////////////////////////////////////////////////////////

void KPUIBase::Initialize(KPnode &rootNode)
{
    pNodes = new KPnodes(rootNode);

    pStatistics = new KPStatistics();
}

void KPUIBase::InitializeAfterOpen()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    glShadeModel(GL_SMOOTH);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glClearColor(0, 0, 0, 0);
    glClearDepth(1.0);
    glDepthFunc(GL_LESS);

    pBoardView = new KPboardView(GetNodes().GetRootNode().GetBoard(),
                                 config.GetDirectory(KP_TEXTURE_DIR),
                                 config.TextureName,
                                 config.TextureSize,
                                 config.Nearest);

    pLight = new Light(config.AmbientLight,
                       config.LightSources);

    pCamera = new Camera();

    pMenu = new KPmenu(config);

    pMenu->Initialize(config.TextureName,
                      config.MenuTextureSize,
                      config.Nearest,
                      config.Language);
#ifdef DEBUG_LIGHT_TEST
    ChangeState(KPState_LightTest);
#else
    ChangeState(KPState_StartUp);
#endif

    InitializeEvents();
}

std::string KPUIBase::GetWindowTitle() const
{
    std::stringstream title;

    title << PACKAGE << " V" << VERSION;

    return title.str();
}

std::string KPUIBase::GetOpenGLVendor() const
{
    const auto *pString = glGetString(GL_VENDOR);
    return (pString != nullptr) ?
           std::string(reinterpret_cast<const char *>(pString)) : "[unknown]";
}

std::string KPUIBase::GetOpenGLRenderer() const
{
    const auto *pString = glGetString(GL_RENDERER);
    return (pString != nullptr) ?
           std::string(reinterpret_cast<const char *>(pString)) : "[unknown]";
}

std::string KPUIBase::GetOpenGLVersion() const
{
    const auto *pString = glGetString(GL_VERSION);
    return (pString != nullptr) ?
           std::string(reinterpret_cast<const char *>(pString)) : "[unknown]";
}

void KPUIBase::DebugPrintOpenGLVersion() const
{
    BLogger::Log("OpenGL Vendor:   ", GetOpenGLVendor());
    BLogger::Log("OpenGL Renderer: ", GetOpenGLRenderer());
    BLogger::Log("OpenGL Version:  ", GetOpenGLVersion());
#ifdef GLU_VERSION_1_3
    BLogger::Log("GLU Header Version 1.3");
#else
#ifdef GLU_VERSION_1_2
    BLogger::Log("GLU Header Version 1.2");
#else
#ifdef GLU_VERSION_1_1
    BLogger::Log("GLU Header Version 1.1");
#else
    BLogger::Log("GLU Header Version 1.0");
#endif
#endif
#endif
#ifdef GLU_VERSION_1_1
    BLogger::Log("GLU Linked Version ", gluGetString(GLU_VERSION));
#endif
}

/////////////////////////////////////////////////////////////////////
// Event Handling
/////////////////////////////////////////////////////////////////////

void KPUIBase::Display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (config.DisplayFPS)
    {
        DisplayFPS();
    }

    pState->Draw(this);  // Drawing is delegated to KPstate
    glFinish();

    SwapBuffers();
}

void KPUIBase::Reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GetCamera().SetAspectRatio(width / static_cast<float>(height));
}

void KPUIBase::Idle()
{
    if (lastFrameTimestamp == 0)
    {
        // first time initialization
        lastFrameTimestamp = fpsTime.GetTimeMsl();
        return;
    }

    unsigned long frameTimestamp = fpsTime.GetTimeMsl();
    // duration is the time difference to the previous cyclic update in ms
    unsigned long duration = frameTimestamp - lastFrameTimestamp;

    if (duration >= 10)
    {
        // Animate approx. each 10 ms
        // Animation is delegated to KPstate
        pState->AnimateAll(this, duration);

        lastFrameTimestamp = frameTimestamp;

        PostWindowRedisplay();
    }
}

void KPUIBase::DisplayFPS()
{
    auto t = fpsTime.GetTimeMsl();

    frameCount++;

    if (t - oldTime > 1000)  // update every second
    {
        pMenu->UpdateFPS(frameCount,
                         static_cast<float>(t - oldTime) / frameCount);
        oldTime    = t;
        frameCount = 0;
    }
}

void KPUIBase::KeyPressed(unsigned char keyPressed, int x, int y)
{
    pState->KeyPressed(this, keyPressed, x, y);
}

/////////////////////////////////////////////////////////////////////
// Implementation of State interface
/////////////////////////////////////////////////////////////////////

void KPUIBase::ChangeState(tKPMenuState stateID)
{
    if (stateID != KPState_Invalid)
    {
        KPstate *pPreviousState = pState;

        pState = KPstateFactory::CreateState(stateID);
        pState->Initialize(this, pPreviousState);
        delete pPreviousState;
    }
}

tKPMenuState KPUIBase::GetPreviousState() const
{
    return previousStateId;
}

void KPUIBase::SetPreviousState(tKPMenuState stateID)
{
    previousStateId = stateID;
}

KPboardView &KPUIBase::GetBoardView()
{
    return *pBoardView;
}
Camera &KPUIBase::GetCamera()
{
    return *pCamera;
}
Light &KPUIBase::GetLight()
{
    return *pLight;
}
KPmenu &KPUIBase::GetMenu()
{
    return *pMenu;
}
KPStatistics &KPUIBase::GetStatistics()
{
    return *pStatistics;
}
KPnodes &KPUIBase::GetNodes()
{
    return *pNodes;
}

KPUIBase &KPUIBase::GetUserInterface()
{
    return *this;
}

BManualTimer &KPUIBase::GetAnimationTimer()
{
    return animationTimer;
}

KPConfig &KPUIBase::GetConfig()
{
    return config;
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
