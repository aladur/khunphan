/*
    kpuibase.cpp

    Automatic solution finder for KhunPhan game
    Copyright (C) 2001-2024  W. Schwotzer

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
#include <memory>
#include "kpuibase.h"
#include "kpstatefactory.h"
#include "kpboardGL.h"
#include "camera.h"
#include "light.h"
#include "kpstate.h"
#include "kpstatistics.h"
#include "kpnodes.h"
#include "blogger.h"

// Uncomment the following line to
// compile for a specific light test version
// #define DEBUG_LIGHT_TEST

// pInstance contains the non-owning pointer to an instance of a
// subclass of KPUIBase.
KPUIBase *KPUIBase::pInstance = nullptr;

KPUIBase::KPUIBase(KPConfigPtr PConfig) :
    animationTimer(TOTAL_ANIMATIONTIME, false),
    previousStateId(StateId::Invalid), isPause(false),
    config(PConfig),
    lastFrameTimestamp(0U), oldTime(0U), frameCount(0)

{
    // the instance is needed for callback handling.
    // Concequence: Only one UserInterface instance is
    // supported at a time
    if (pInstance != NULL)
    {
        throw std::runtime_error(
            "Error: Only one UserInterface instance allowed at a time");
    }
    else
    {
        pInstance = this;
    }
}

KPUIBase::~KPUIBase()
{
    pInstance = nullptr;
}

/////////////////////////////////////////////////////////////////////
// Public Interface
/////////////////////////////////////////////////////////////////////

void KPUIBase::Initialize(KPnode &rootNode)
{
    pNodes = std::make_unique<KPnodes>(rootNode);

    pStatistics = std::make_unique<KPStatistics>();
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

    pBoardView = std::make_unique<KPboardView>(
                     GetNodes().GetRootNode().GetBoard(),
                     config->Get(KPDirectory::Texture),
                     config->TextureName,
                     config->TextureSize,
                     config->Nearest);

    pLight = std::make_unique<Light>(config->AmbientLight,
                                     config->LightSources);

    pCamera = std::make_unique<Camera>();

    pMenu = std::make_unique<KPmenu>(config);

    pMenu->Initialize(config->TextureName,
                      config->MenuTextureSize,
                      config->Nearest,
                      static_cast<Signal>(config->Language));
#ifdef DEBUG_LIGHT_TEST
    ChangeState(StateId::LightTest);
#else
    ChangeState(StateId::StartUp);
#endif

    InitializeEvents();

    // Starting point for calculating frames per second
    oldTime = fpsTime.GetTimeMsll();
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

    if (config->DisplayFPS)
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
    if (lastFrameTimestamp == 0U)
    {
        // first time initialization
        lastFrameTimestamp = fpsTime.GetTimeMsll();
        return;
    }

    auto frameTimestamp = fpsTime.GetTimeMsll();
    // duration is the time difference to the previous cyclic update in ms
    auto duration = frameTimestamp - lastFrameTimestamp;

    if (duration >= 10U)
    {
        // Animate approx. each 10 ms
        // Animation is delegated to KPstate
        pState->AnimateAll(this, static_cast<unsigned int>(duration));

        lastFrameTimestamp = frameTimestamp;

        PostWindowRedisplay();
    }
}

void KPUIBase::DisplayFPS()
{
    auto t = fpsTime.GetTimeMsll();

    frameCount++;

    if (t - oldTime > 1000U)  // update every second
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

void KPUIBase::ChangeState(StateId stateID)
{
    if (stateID != StateId::Invalid)
    {
        auto previousStateID = pState ? pState->GetId() : StateId::Invalid;

        pState = KPstateFactory::CreateState(stateID);
        pState->Initialize(this, previousStateID);
    }
}

StateId KPUIBase::GetPreviousState() const
{
    return previousStateId;
}

void KPUIBase::SetPreviousState(StateId stateID)
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
    return *config;
}

/////////////////////////////////////////////////////////////////////
// Implementation of Audio interface
/////////////////////////////////////////////////////////////////////

bool KPUIBase::InitializeAudio(const std::string & /* textureName */)
{
    // default implementation: No Audio support, no error
    return true;
}

void KPUIBase::PlayAudio(KPSound /* soundId */) const
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
