/*
    kpuibase.h

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

#ifndef KPUIBASE_H
#define KPUIBASE_H

#include <string>
#include "misc1.h"
#include "KPstateContext.h"
#include "kpmenu.h"
#include "btime.h"


class KPboard;
class KPnodes;
class KPboardView;
class Camera;
class Light;
class KPStatistics;

enum tGetValues
{
    KP_WINDOW_WIDTH  = 88,
    KP_WINDOW_HEIGHT = 89
};

enum tKPSoundIDs
{
    KP_SND_OPENMENU       = 0,
    KP_SND_QUITGAME       = 1,
    KP_SND_MOVETOKEN      = 2,
    KP_SND_CHANGESETTING  = 3,
    KP_SND_TUTORIALNEXT   = 4,
    KP_SND_OPENGAME       = 5,
    KP_SND_GAMESOLVED     = 6,
    KP_SND_MAX            = 7
};

class KPUIBase : public KPstateContext
{
public:
    // public interface
    KPUIBase();
    virtual ~KPUIBase();

    virtual bool OpenWindow(int argc, char **argv) = 0;
    virtual void SetWindowMode(bool FullScreen) const = 0;
    virtual void MainLoop() = 0;
    virtual void Close() = 0;
    virtual int  GetValue(int what) const = 0;
    virtual bool CanToggleFullScreen() const
    {
        return false;
    };
    virtual std::string GetWindowTitle() const;

    bool IsInitialized();
    void DebugPrintOpenGLVersion()  const;
    std::string GetOpenGLVendor()   const;
    std::string GetOpenGLRenderer() const;
    std::string GetOpenGLVersion()  const;

    // member functions for event handling
    void MouseMotion(int x, int y);
    void KeyPressed ( unsigned char keyPressed,  int x, int y );
    void KeyReleased( unsigned char keyReleased, int x, int y );
    void Idle();
    void Visible (int vis) const;
    void Display();
    void Reshape(int x, int y);
    virtual void Timer(int value) = 0;
    virtual void MouseClick(int button, int state,
                            int x, int y) = 0;

protected:
    void Initialize(KPboard &rootBoard);
    void DisplayFPS();
    void InitializeAfterOpen();

    virtual void InitializeEvents() = 0;
    virtual void SwapBuffers() = 0;
    virtual void PostWindowRedisplay() = 0;

    BTime   fpsTime;

// Sw: unfinished: should be protected
public:
    KPboardView  *pBoardView;
    Camera       *pCamera;
    Light        *pLight;
    KPmenu       *pMenu;
    KPnodes      *pNodes;
    KPStatistics *pStatistics;

protected:
    KPstate *pState;
    static  KPUIBase *instance;

private:
    unsigned long lastFrameTimestamp;
    unsigned long oldTime;
    unsigned int frameCount;

// Audio interface
public:
    virtual bool InitializeAudio(const char *textureName,
                                 bool reInitialize = false);
    virtual void SetSoundVolume(int volume) const;
    virtual void SetMusicVolume(int volume) const;
    virtual void PlayAudio(int soundId) const;
    virtual void PlayMusic(bool On, bool resetPos = false);
    virtual void StopMusicCallback();

// KPstateContext interface
public:
    virtual void ChangeState( int stateID );
    virtual KPboardView     &GetBoardView();
    virtual Camera          &GetCamera();
    virtual Light           &GetLight();
    virtual KPmenu          &GetMenu();
    virtual KPnodes         &GetNodes();
    virtual KPStatistics    &GetStatistics();
    virtual KPUIBase        &GetUserInterface();
};

#endif
