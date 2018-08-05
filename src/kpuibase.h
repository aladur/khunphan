/*
    kpuibase.h

    Automatic solution finder for KhunPhan game
    Copyright (C) 2001-2018  W. Schwotzer

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

#ifndef KPUIBASE_H
#define KPUIBASE_H

#include "stdafx.h"
#include <memory>
#include <string>
#include "KPstateContext.h"
#include "kpmenu.h"
#include "btime.h"
#include "bmanualtimer.h"
#include "kpconfig.h"


class KPnode;
class KPnodes;
class KPboardView;
class Camera;
class Light;
class KPStatistics;

enum class WindowProperty
{
    Width,
    Height
};

enum class KPSound
{
    OpenMenu,
    QuitGame,
    MoveToken,
    ChangeSetting,
    TutorialNext,
    OpenGame,
    GameSolved,
    COUNT    // This should be always the last entry
};

class KPUIBase : public KPstateContext
{
public:
    KPUIBase(KPConfigPtr PConfig);
    virtual ~KPUIBase();

    virtual void OpenWindow(int argc, char **argv) = 0;
    virtual void SetWindowMode(bool FullScreen) const = 0;
    virtual void SetWindowSize(int width, int height) const = 0;
    virtual void MainLoop() = 0;
    virtual void Close() = 0;
    virtual void RequestForClose() = 0;
    virtual int  Get(WindowProperty what) const = 0;
    virtual bool IsWindowResolutionSupported(int width, int height) const = 0;
    virtual bool CanToggleFullScreen() const
    {
        return false;
    };
    virtual bool CanChangeWindowSize() const
    {
        return false;
    };
    std::string GetWindowTitle() const;

    void DebugPrintOpenGLVersion()  const;
    std::string GetOpenGLVendor()   const;
    std::string GetOpenGLRenderer() const;
    std::string GetOpenGLVersion()  const;

    // member functions for event handling
    void KeyPressed(unsigned char keyPressed, int x, int y);
    void Idle();
    void Display();
    void Reshape(int width, int height);
    virtual void MouseClick(int button, int state,
                            int x, int y) = 0;

protected:
    void Initialize(KPnode &rootNode);
    void DisplayFPS();
    void InitializeAfterOpen();

    virtual void InitializeEvents() = 0;
    virtual void SwapBuffers() = 0;
    virtual void PostWindowRedisplay() = 0;

    BTime   fpsTime;

private:
    std::unique_ptr<KPboardView> pBoardView;
    std::unique_ptr<Camera> pCamera;
    std::unique_ptr<Light> pLight;
    std::unique_ptr<KPmenu> pMenu;
    std::unique_ptr<KPnodes> pNodes;
    std::unique_ptr<KPStatistics> pStatistics;
    BManualTimer animationTimer;
    StateId previousStateId;
    bool isPause;

protected:
    std::unique_ptr<KPstate> pState;
    KPConfigPtr config;
    static KPUIBase *pInstance;

private:
    unsigned long lastFrameTimestamp;
    unsigned long oldTime;
    unsigned int frameCount;

    // Audio interface
public:
    virtual bool InitializeAudio(const std::string &textureName);
    virtual void SetSoundVolume(int volume) const;
    virtual void SetMusicVolume(int volume) const;
    virtual void PlayAudio(KPSound soundId) const;
    virtual void PlayMusic(bool On, bool resetPos = false);
    virtual void StopMusicCallback();

    // KPstateContext interface
public:
    virtual void ChangeState(StateId stateID);
    virtual StateId GetPreviousState() const;
    virtual void SetPreviousState(StateId stateID);
    virtual void SetPause(bool on)
    {
        isPause = on;
    }
    virtual bool IsPause()
    {
        return isPause;
    }

    virtual KPboardView     &GetBoardView();
    virtual Camera          &GetCamera();
    virtual Light           &GetLight();
    virtual KPmenu          &GetMenu();
    virtual KPnodes         &GetNodes();
    virtual KPStatistics    &GetStatistics();
    virtual KPUIBase        &GetUserInterface();
    virtual BManualTimer    &GetAnimationTimer();
    virtual KPConfig        &GetConfig();
};

#endif
