/*
    kpstategame.cpp

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
#include "kpstategame.h"
#include "kpmenu.h"
#include "kpconfig.h"
#include "kpboardGL.h"
#include "Kamera.h"
#include "kpstatistics.h"
#include "kpuibase.h"


KPstateGame::KPstateGame() : animationFinishedCallback(NULL),
    bIsPause(false), bGameIsSolved(false), counter(0)
{
}

void KPstateGame::Initialize(KPstateContext *pContext,
                             const KPstate *pOldState)
{
    KPstate::Initialize(pContext, pOldState);

    // Do some initialization stuff here:

    pContext->GetStatistics().Reset();
    if (KPConfig::Instance().SavedGame != 0)
    {
        // In this case restore a already started game
        pContext->GetBoardView().SetBoard(KPConfig::Instance().SavedGame);
        pContext->GetStatistics().SetPlayingTime(KPConfig::Instance().PlayTime);
        pContext->GetStatistics().SetEventCounter(MOVE_COUNTER,
                KPConfig::Instance().Moves        );
        pContext->GetStatistics().SetEventCounter(MOVE_WITH_HELP_CNT,
                KPConfig::Instance().MovesWithHint);
        pContext->GetStatistics().SetEventCounter(USED_CHEATS_CNT,
                KPConfig::Instance().CheatCount   );
    }
    else
    {
        pContext->GetBoardView().ResetBoard();
    }

    pContext->GetCamera().ladePosition(KPConfig::Instance().CameraPosition);
    pContext->GetStatistics().Start();

    UpdateDisplay(pContext);
}

void KPstateGame::UpdateDisplay(KPstateContext *pContext)
{
    KPstate::UpdateDisplay(pContext);

    KPmenu &menu = pContext->GetMenu();

    pContext->GetUserInterface().SetSoundVolume(KPConfig::Instance().SoundOn ?
            KPConfig::Instance().SoundVolume : 0);
    pContext->GetUserInterface().PlayMusic(KPConfig::Instance().MusicOn, false);

    menu.SchildArray[SHLD_LOGO].Positioniere(0,11,4,12);
    menu.SchildArray[SHLD_LOGO].SetzeSignal(S_LOGO);

    pContext->GetMenu().TextfeldArray[T_MINMOVECOUNT].FormatText(NULL,
            pContext->GetBoardView().GetMovesToSolve());
    pContext->GetMenu().TextfeldArray[T_MOVECOUNT   ].FormatText(NULL,
            pContext->GetStatistics().GetEventCounter(MOVE_COUNTER));

    if (KPConfig::Instance().SolutionHint)
    {
        menu.TextfeldArray[T_MINMOVECOUNT].Positioniere(7,0.1f,0.6f);
    }

    menu.TextfeldArray[T_MOVECOUNT].Positioniere(4,0.1f,0.6f);
    menu.TextfeldArray[T_TIME     ].Positioniere(12.5,0.1f,0.6f);

    if (KPConfig::Instance().UserInterface == 0)
    {
        if (KPConfig::Instance().SoundOn)
        {
            menu.SchildArray[SHLD_SOUND_ON].Positioniere(0.3f,0.1f,1,0.5);
            menu.SchildArray[SHLD_SOUND_ON].SetzeSignal(S_TOGGLE_SOUND_ON);
            menu.SchildArray[SHLD_SOUND_ON].VollSichtbar();
        }
        else
        {
            menu.SchildArray[SHLD_SOUND_OFF].Positioniere(0.3f,0.1f,1,0.5);
            menu.SchildArray[SHLD_SOUND_OFF].SetzeSignal(S_TOGGLE_SOUND_ON);
            menu.SchildArray[SHLD_SOUND_OFF].VollSichtbar();
        }
        if (KPConfig::Instance().MusicOn)
        {
            menu.SchildArray[SHLD_MUSIC_ON].Positioniere(1.0f,0.1f,1.6f,0.5);
            menu.SchildArray[SHLD_MUSIC_ON].SetzeSignal(S_TOGGLE_MUSIC_ON);
            menu.SchildArray[SHLD_MUSIC_ON].VollSichtbar();
        }
        else
        {
            menu.SchildArray[SHLD_MUSIC_OFF].Positioniere(1.0f,0.1f,1.6f,0.5);
            menu.SchildArray[SHLD_MUSIC_OFF].SetzeSignal(S_TOGGLE_MUSIC_ON);
            menu.SchildArray[SHLD_MUSIC_OFF].VollSichtbar();
        }
    }

    StartAnimation();
}

void  KPstateGame::KeyPressed (KPstateContext *pContext, unsigned char key,
                               int x, int y)
{
    if (bIsPause)
    {
        if (key == 'P' || key == 'p' || key == 27)
        {
            Pause(pContext, false);
            if (KPConfig::Instance().MusicOn)
            {
                pContext->GetUserInterface().PlayMusic(true, false);
            }
            pContext->GetStatistics().Start();
        }
        return;
    }

    CHECK_DEFAULT_KEY_PRESSED(pContext, key, x, y);

    switch (key)
    {
        // Hidden feature for test purposes:
        // move tokens to one position before finishing the game
        // means Ctrl+X
        case 'X' - 'A' + 1:
            Cheat1(pContext);
            break;

        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
            KPConfig::Instance().CameraPosition = key - '1' + 1;
            KPConfig::Instance().WriteToFile();
            pContext->GetCamera().ladePosition(
                KPConfig::Instance().CameraPosition);
            break;
        case 'P':
        case 'p':
            Pause(pContext, true);
            pContext->GetUserInterface().PlayMusic(false);
            pContext->GetStatistics().Stop();
            break;
    }
}

tKPMenuState KPstateGame::ESCKeyAction (KPstateContext *pContext)
{
    pContext->GetCamera().SetzeRundflug(true);
    pContext->GetStatistics().Stop();
    SaveGameStatus(pContext);
    pContext->GetUserInterface().SetSoundVolume(
        KPConfig::Instance().SoundVolume);
    pContext->GetUserInterface().PlayMusic(false);
    return KPState_MainMenu;
}

void  KPstateGame::MouseClick (KPstateContext *pContext,
                               tMouseButton button, tMouseEvent event,
                               int x, int y)
{
    int Signal = KPstate::EvaluateMouseClick(pContext, button, event, x, y);

    if (!bIsPause && !bGameIsSolved)
    {
        switch (Signal)
        {
            case S_LOGO:
                pContext->GetCamera().SetzeRundflug(true);
                pContext->GetStatistics().Stop();
                SaveGameStatus(pContext);
                pContext->GetUserInterface().SetSoundVolume(
                    KPConfig::Instance().SoundVolume);
                pContext->GetUserInterface().PlayMusic(false);
                pContext->ChangeState(KPState_MainMenu);
                break;
            case S_TOGGLE_SOUND_ON:
                KPConfig::Instance().SoundOn = !KPConfig::Instance().SoundOn;
                KPConfig::Instance().WriteToFile();
                UpdateDisplay(pContext);
                break;
            case S_TOGGLE_MUSIC_ON:
                KPConfig::Instance().MusicOn = !KPConfig::Instance().MusicOn;
                KPConfig::Instance().WriteToFile();
                UpdateDisplay(pContext);
                break;
            default:
                MouseMoveToken(pContext, button, event, x, y);
                break;
        }
    }
}

void KPstateGame::HookAfterTokenMoved(KPstateContext *pContext, tKPTokenID id,
                                      tKPDirection, bool successfullyMoved)
{
    if (successfullyMoved && id != TK_EMPTY)
    {
        // The token actually is moved
        pContext->GetUserInterface().PlayAudio(KP_SND_MOVETOKEN);
        UpdateMoveCount(pContext);
        if (pContext->GetBoardView().IsSolved())
        {
            bGameIsSolved = true;
            pContext->GetStatistics().Stop();
            animationFinishedCallback = &KPstateGame::GameIsSolved;
            StartAnimation();
        }
    }
}

void KPstateGame::HookAfterAnimationFinished(KPstateContext *pContext)
{
    if (animationFinishedCallback != NULL)
    {
        (this->*animationFinishedCallback)(pContext);
    }
}

void KPstateGame::Update(KPstateContext *pContext, int factor)
{
    KPstate::Update(pContext, factor);

    counter += 16 * factor;
    if (counter > TOTAL_ANIMATIONTIME)
    {
        // update approx. 8 times per second
        counter -= TOTAL_ANIMATIONTIME;
        pContext->GetMenu().TextfeldArray[T_TIME].FormatText(NULL,
                pContext->GetStatistics().GetTotalTime(RTIME_HHMMSS).c_str());
    }
}

void KPstateGame::GameIsSolved(KPstateContext *pContext)
{
    animationFinishedCallback = NULL;
    pContext->GetUserInterface().SetSoundVolume(
        KPConfig::Instance().SoundVolume);
    pContext->GetUserInterface().PlayMusic(false);
    pContext->ChangeState(KPState_GameSolved);
}

void KPstateGame::Pause(KPstateContext *pContext, bool On /* = true */)
{
    KPmenu &menu = pContext->GetMenu();

    bIsPause = On;

    if (bIsPause)
    {
        menu.SchildArray[SHLD_LOGO].Positioniere(4,9,12,11);
        menu.SchildArray[SHLD_SHADER].Positioniere(0,0,16,12);
        menu.SchildArray[SHLD_SHADER].Eingeblendet();
        menu.TextfeldArray[T_PAUSE].Positioniere(8,6,3,A_MITTE);
        menu.TextfeldArray[T_PAUSE].VollSichtbar();
    }
    else
    {
        menu.SchildArray[SHLD_LOGO].Positioniere(0,11,4,12);
        menu.SchildArray[SHLD_LOGO].SetzeSignal(S_LOGO);
        menu.SchildArray[SHLD_SHADER].Desaktiviere();
        menu.TextfeldArray[T_PAUSE].Desaktiviere();
    }
    StartAnimation();
}

void KPstateGame::UpdateMoveCount(KPstateContext *pContext)
{
    pContext->GetStatistics().IncEventCounter(MOVE_COUNTER);
    if (KPConfig::Instance().SolutionHint)
    {
        pContext->GetStatistics().IncEventCounter(MOVE_WITH_HELP_CNT);
    }

    UpdateDisplay(pContext);
}

void KPstateGame::Cheat1(KPstateContext *pContext)
{
    KPnode n;

    n.InitializeToken(TK_GREEN1, "green", 0, 0, 1, 1);
    n.InitializeToken(TK_GREEN2, "green", 3, 3, 1, 1);
    n.InitializeToken(TK_GREEN3, "green", 3, 4, 1, 1);
    n.InitializeToken(TK_GREEN4, "green", 1, 2, 1, 1);
    n.InitializeToken(TK_WHITE1, "white", 0, 1, 1, 2);
    n.InitializeToken(TK_WHITE2, "white", 1, 0, 1, 2);
    n.InitializeToken(TK_WHITE3, "white", 2, 0, 1, 2);
    n.InitializeToken(TK_WHITE4, "white", 3, 0, 1, 2);
    n.InitializeToken(TK_WHITE5, "white", 2, 2, 2, 1);
    n.InitializeToken(TK_RED1,   "red",   0, 3, 2, 2);

    pContext->GetBoardView().SetBoard(&n);
    pContext->GetStatistics().IncEventCounter(USED_CHEATS_CNT);
}

void KPstateGame::SaveGameStatus(KPstateContext *pContext)
{
    KPConfig::Instance().SavedGame = pContext->GetBoardView().GetBoardId();
    KPConfig::Instance().PlayTime = pContext->GetStatistics().GetTotalTime();
    KPConfig::Instance().Moves =
        pContext->GetStatistics().GetEventCounter(MOVE_COUNTER);
    KPConfig::Instance().MovesWithHint =
        pContext->GetStatistics().GetEventCounter(MOVE_WITH_HELP_CNT);
    KPConfig::Instance().CheatCount =
        pContext->GetStatistics().GetEventCounter(USED_CHEATS_CNT);
}

void KPstateGame::PlayAudioForInitialize(KPstateContext *pContext)
{
    pContext->GetUserInterface().PlayAudio(KP_SND_OPENGAME);
}
