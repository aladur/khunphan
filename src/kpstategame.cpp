/*
    kpstategame.cpp

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


#include "misc1.h"
#include "kpstategame.h"
#include "kpmenu.h"
#include "kpconfig.h"
#include "kpboardGL.h"
#include "camera.h"
#include "kpstatistics.h"
#include "kpuibase.h"
#include "kpnodes.h"


KPstateGame::KPstateGame() : animationFinishedCallback(NULL),
    bIsPause(false), bGameIsSolved(false), counter(0)
{
}

void KPstateGame::Initialize(KPstateContext *pContext,
                             const KPstate *pOldState)
{
    KPstate::Initialize(pContext, pOldState);

    KPConfig &config = KPConfig::Instance();
    KPStatistics &statistics = pContext->GetStatistics();

    // Do some initialization stuff here:

    statistics.Reset();
    if (config.SavedGame != 0)
    {
        // In this case restore a already started game
        const KPnode &node = pContext->GetNodes().GetNodeFor(config.SavedGame);
        pContext->GetBoardView().SetBoard(node.GetBoard());
        statistics.SetPlayingTime(config.PlayTime);
        statistics.SetEventCounter(MOVE_COUNTER, config.Moves);
        statistics.SetEventCounter(MOVE_WITH_HELP_CNT, config.MovesWithHint);
        statistics.SetEventCounter(USED_CHEATS_CNT, config.CheatCount);
    }
    else
    {
        const KPnode &node = pContext->GetNodes().GetRootNode();
        pContext->GetBoardView().SetBoard(node.GetBoard());
    }

    pContext->GetCamera().SetPosition(config.CameraPosition);
    statistics.Start();

    UpdateDisplay(pContext);
}

void KPstateGame::UpdateDisplay(KPstateContext *pContext)
{
    KPstate::UpdateDisplay(pContext);

    KPmenu &menu     = pContext->GetMenu();
    KPConfig &config = KPConfig::Instance();

    pContext->GetUserInterface().SetSoundVolume(config.SoundOn ?
            config.SoundVolume : 0);
    pContext->GetUserInterface().PlayMusic(config.MusicOn, false);

    menu.plates[PLATE_LOGO].SetPosition(0,11,4,12);
    menu.plates[PLATE_LOGO].SetSignal(S_LOGO);

    QWord id = pContext->GetBoardView().GetBoardId();
    int movesToSolve = pContext->GetNodes().GetNodeFor(id).GetMovesToSolve();
    menu.labels[T_MINMOVECOUNT].FormatText(1, movesToSolve);
    menu.labels[T_MOVECOUNT].FormatText(
            1, pContext->GetStatistics().GetEventCounter(MOVE_COUNTER));

    if (config.SolutionHint)
    {
        menu.labels[T_MINMOVECOUNT].SetPosition(7,0.1f,0.6f);
    }

    menu.labels[T_MOVECOUNT].SetPosition(4,0.1f,0.6f);
    menu.labels[T_TIME].SetPosition(12.5,0.1f,0.6f);

    if (config.UserInterface == 0)
    {
        if (config.SoundOn)
        {
            menu.plates[PLATE_SOUND_ON].SetPosition(0.3f,0.1f,1,0.5);
            menu.plates[PLATE_SOUND_ON].SetSignal(S_TOGGLE_SOUND_ON);
            menu.plates[PLATE_SOUND_ON].SetFullyVisible();
        }
        else
        {
            menu.plates[PLATE_SOUND_OFF].SetPosition(0.3f,0.1f,1,0.5);
            menu.plates[PLATE_SOUND_OFF].SetSignal(S_TOGGLE_SOUND_ON);
            menu.plates[PLATE_SOUND_OFF].SetFullyVisible();
        }
        if (config.MusicOn)
        {
            menu.plates[PLATE_MUSIC_ON].SetPosition(1.0f,0.1f,1.6f,0.5);
            menu.plates[PLATE_MUSIC_ON].SetSignal(S_TOGGLE_MUSIC_ON);
            menu.plates[PLATE_MUSIC_ON].SetFullyVisible();
        }
        else
        {
            menu.plates[PLATE_MUSIC_OFF].SetPosition(1.0f,0.1f,1.6f,0.5);
            menu.plates[PLATE_MUSIC_OFF].SetSignal(S_TOGGLE_MUSIC_ON);
            menu.plates[PLATE_MUSIC_OFF].SetFullyVisible();
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
            pContext->GetCamera().SetPosition(
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
    pContext->GetCamera().SetRoundtrip(true);
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
    KPConfig &config = KPConfig::Instance();

    int Signal = KPstate::EvaluateMouseClick(pContext, button, event, x, y);

    if (!bIsPause && !bGameIsSolved)
    {
        switch (Signal)
        {
            case S_LOGO:
                pContext->GetCamera().SetRoundtrip(true);
                pContext->GetStatistics().Stop();
                SaveGameStatus(pContext);
                pContext->GetUserInterface().SetSoundVolume(config.SoundVolume);
                pContext->GetUserInterface().PlayMusic(false);
                pContext->ChangeState(KPState_MainMenu);
                break;
            case S_TOGGLE_SOUND_ON:
                config.SoundOn = !config.SoundOn;
                config.WriteToFile();
                UpdateDisplay(pContext);
                break;
            case S_TOGGLE_MUSIC_ON:
                config.MusicOn = !config.MusicOn;
                config.WriteToFile();
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
        pContext->GetMenu().labels[T_TIME].FormatText(
               1, pContext->GetStatistics().GetTotalTime(RTIME_HHMMSS).c_str());
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
        menu.plates[PLATE_LOGO].SetPosition(4,9,12,11);
        menu.plates[PLATE_SHADER].SetPosition(0,0,16,12);
        menu.plates[PLATE_SHADER].SetFadeIn();
        menu.labels[T_PAUSE].SetPosition(8,6,3,A_CENTERED);
        menu.labels[T_PAUSE].SetFullyVisible();
    }
    else
    {
        menu.plates[PLATE_LOGO].SetPosition(0,11,4,12);
        menu.plates[PLATE_LOGO].SetSignal(S_LOGO);
        menu.plates[PLATE_SHADER].SetFadeOut();
        menu.labels[T_PAUSE].SetFadeOut();
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
    KPboard board;

    board.InitializeToken(TK_GREEN1, 0, 0);
    board.InitializeToken(TK_GREEN2, 3, 3);
    board.InitializeToken(TK_GREEN3, 3, 4);
    board.InitializeToken(TK_GREEN4, 1, 2);
    board.InitializeToken(TK_WHITE1, 0, 1);
    board.InitializeToken(TK_WHITE2, 1, 0);
    board.InitializeToken(TK_WHITE3, 2, 0);
    board.InitializeToken(TK_WHITE4, 3, 0);
    board.InitializeToken(TK_WHITE5, 2, 2);
    board.InitializeToken(TK_RED1,   0, 3);

    pContext->GetBoardView().SetBoard(board);
    pContext->GetStatistics().IncEventCounter(USED_CHEATS_CNT);
}

void KPstateGame::SaveGameStatus(KPstateContext *pContext)
{
    KPConfig &config = KPConfig::Instance();
    KPStatistics &statistics = pContext->GetStatistics();

    config.SavedGame     = pContext->GetBoardView().GetBoardId();
    config.PlayTime      = statistics.GetTotalTime();
    config.Moves         = statistics.GetEventCounter(MOVE_COUNTER);
    config.MovesWithHint = statistics.GetEventCounter(MOVE_WITH_HELP_CNT);
    config.CheatCount    = statistics.GetEventCounter(USED_CHEATS_CNT);
}

void KPstateGame::PlayAudioForInitialize(KPstateContext *pContext)
{
    pContext->GetUserInterface().PlayAudio(KP_SND_OPENGAME);
}
