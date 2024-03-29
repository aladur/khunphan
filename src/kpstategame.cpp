/*
    kpstategame.cpp

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
#include "kpstategame.h"
#include "kpmenu.h"
#include "kpconfig.h"
#include "kpboardGL.h"
#include "camera.h"
#include "kpstatistics.h"
#include "kpuibase.h"
#include "kpnodes.h"


void KPstateGame::Initialize(KPstateContext *pContext,
                             StateId previousStateId)
{
    KPstate::Initialize(pContext, previousStateId);

    auto &config = pContext->GetConfig();
    auto &statistics = pContext->GetStatistics();

    pContext->SetPause(false);

    statistics.Reset();

    if (config.SavedGame != 0)
    {
        // In this case restore a already started game
        const auto &node = pContext->GetNodes().GetNodeFor(config.SavedGame);
        pContext->GetBoardView().SetBoard(node.GetBoard());
        statistics.SetPlayingTime(config.PlayTime);
        statistics.Set(EventCounter::Moves, config.Moves);
        statistics.Set(EventCounter::MovesWithHint, config.MovesWithHint);
        statistics.Set(EventCounter::MovesWithCheat, config.MovesWithCheat);
    }
    else
    {
        const auto &node = pContext->GetNodes().GetRootNode();
        pContext->GetBoardView().SetBoard(node.GetBoard());
    }

    pContext->GetCamera().SetPosition(config.CameraPosition);
    statistics.Start();

    UpdateDisplay(pContext);
}

void KPstateGame::UpdateDisplay(KPstateContext *pContext) const
{
    KPstate::UpdateDisplay(pContext);

    auto &menu = pContext->GetMenu();
    auto &config = pContext->GetConfig();

    pContext->GetUserInterface().SetSoundVolume(config.SoundOn ?
            config.SoundVolume : 0);
    pContext->GetUserInterface().PlayMusic(config.MusicOn, false);

    menu.plates[KPPlate::Logo].SetPosition(0, 11, 4, 12);
    menu.plates[KPPlate::Logo].SetSignal(Signal::MainMenu);

    auto id = pContext->GetBoardView().GetBoardId();
    auto movesToSolve = pContext->GetNodes().GetNodeFor(id).GetMovesToSolve();
    menu.labels[Lbl::MoveCountHint].FormatText(movesToSolve);
    menu.labels[Lbl::MoveCount].FormatText(
        pContext->GetStatistics().Get(EventCounter::Moves));

    if (config.SolutionHint)
    {
        menu.labels[Lbl::MoveCountHint].SetPosition(7, 0.1f, 0.6f);
    }

    menu.labels[Lbl::MoveCount].SetPosition(4, 0.1f, 0.6f);
    menu.labels[Lbl::Time].SetPosition(12.5, 0.1f, 0.6f);

    if (config.UserInterface == 0)
    {
        if (config.SoundOn)
        {
            menu.plates[KPPlate::SoundOn].SetPosition(0.3f, 0.1f, 1, 0.5);
            menu.plates[KPPlate::SoundOn].SetSignal(Signal::ToggleSoundOn);
            menu.plates[KPPlate::SoundOn].SetFullyVisible();
        }
        else
        {
            menu.plates[KPPlate::SoundOff].SetPosition(0.3f, 0.1f, 1, 0.5);
            menu.plates[KPPlate::SoundOff].SetSignal(Signal::ToggleSoundOn);
            menu.plates[KPPlate::SoundOff].SetFullyVisible();
        }

        if (config.MusicOn)
        {
            menu.plates[KPPlate::MusicOn].SetPosition(1.0f, 0.1f, 1.6f, 0.5);
            menu.plates[KPPlate::MusicOn].SetSignal(Signal::ToggleMusicOn);
            menu.plates[KPPlate::MusicOn].SetFullyVisible();
        }
        else
        {
            menu.plates[KPPlate::MusicOff].SetPosition(1.0f, 0.1f, 1.6f, 0.5);
            menu.plates[KPPlate::MusicOff].SetSignal(Signal::ToggleMusicOn);
            menu.plates[KPPlate::MusicOff].SetFullyVisible();
        }
    }

    StartAnimation(pContext);
}

void  KPstateGame::KeyPressed(KPstateContext *pContext, unsigned char key,
                              int x, int y) const
{
    if (pContext->IsPause())
    {
        if (key == 'P' || key == 'p' || key == 27)
        {
            Pause(pContext, false);

            if (pContext->GetConfig().MusicOn)
            {
                pContext->GetUserInterface().PlayMusic(true, false);
            }

            pContext->GetStatistics().Start();
        }

        return;
    }

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
            pContext->GetConfig().CameraPosition = key - '1' + 1;
            pContext->GetConfig().WriteToFile();
            pContext->GetCamera().SetPosition(
                pContext->GetConfig().CameraPosition);
            break;

        case 'P':
        case 'p':
            Pause(pContext, true);
            pContext->GetUserInterface().PlayMusic(false);
            pContext->GetStatistics().Stop();
            break;
    }

    KPstate::KeyPressed(pContext, key, x, y);
}

StateId KPstateGame::ESCKeyAction(KPstateContext *pContext) const
{
    pContext->GetCamera().SetRoundtrip(true);
    pContext->GetStatistics().Stop();
    SaveGameStatus(pContext);
    pContext->GetUserInterface().SetSoundVolume(
        pContext->GetConfig().SoundVolume);
    pContext->GetUserInterface().PlayMusic(false);
    return StateId::MainMenu;
}

void  KPstateGame::MouseClick(KPstateContext *pContext,
                              MouseButton button, MouseButtonEvent event,
                              int x, int y)
{
    auto &config = pContext->GetConfig();

    auto signal = KPstate::EvaluateMouseClick(pContext, button, event, x, y);

    if (!pContext->IsPause() && !pContext->GetBoardView().IsSolved())
    {
        switch (signal)
        {
            case Signal::MainMenu:
                pContext->GetCamera().SetRoundtrip(true);
                pContext->GetStatistics().Stop();
                SaveGameStatus(pContext);
                pContext->GetUserInterface().SetSoundVolume(config.SoundVolume);
                pContext->GetUserInterface().PlayMusic(false);
                pContext->ChangeState(StateId::MainMenu);
                break;

            case Signal::ToggleSoundOn:
                config.SoundOn = !config.SoundOn;
                config.WriteToFile();
                UpdateDisplay(pContext);
                break;

            case Signal::ToggleMusicOn:
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

void KPstateGame::HookAfterTokenMoved(KPstateContext *pContext, TokenId id,
                                      MoveToken, bool successfullyMoved)
{
    if (successfullyMoved && id != TokenId::EMPTY)
    {
        // The token actually is moved
        pContext->GetUserInterface().PlayAudio(KPSound::MoveToken);
        UpdateMoveCount(pContext);

        if (pContext->GetBoardView().IsSolved())
        {
            pContext->GetStatistics().Stop();
            StartAnimation(pContext);
        }
    }
}

void KPstateGame::HookAfterAnimationFinished(KPstateContext *pContext) const
{
    if (pContext->GetBoardView().IsSolved())
    {
        GameIsSolved(pContext);
    }
}

void KPstateGame::AnimateAll(KPstateContext *pContext,
                             unsigned int duration) const
{
    KPstate::AnimateAll(pContext, duration);

    pContext->GetMenu().UpdatePlayTime(pContext, duration);
}

void KPstateGame::GameIsSolved(KPstateContext *pContext) const
{
    pContext->GetUserInterface().SetSoundVolume(
        pContext->GetConfig().SoundVolume);
    pContext->GetUserInterface().PlayMusic(false);
    pContext->ChangeState(StateId::GameSolved);
}

void KPstateGame::Pause(KPstateContext *pContext, bool On /* = true */) const
{
    auto &menu = pContext->GetMenu();

    pContext->SetPause(On);

    if (pContext->IsPause())
    {
        menu.plates[KPPlate::Logo].SetPosition(4, 9, 12, 11);
        menu.plates[KPPlate::Shader].SetPosition(0, 0, 16, 12);
        menu.plates[KPPlate::Shader].SetFadeIn();
        menu.labels[Lbl::Pause].SetPosition(8, 6, 3, AlignItem::Centered);
        menu.labels[Lbl::Pause].SetFullyVisible();
    }
    else
    {
        menu.plates[KPPlate::Logo].SetPosition(0, 11, 4, 12);
        menu.plates[KPPlate::Logo].SetSignal(Signal::MainMenu);
        menu.plates[KPPlate::Shader].SetFadeOut();
        menu.labels[Lbl::Pause].SetFadeOut();
    }

    StartAnimation(pContext);
}

void KPstateGame::UpdateMoveCount(KPstateContext *pContext) const
{
    pContext->GetStatistics().Increment(EventCounter::Moves);

    if (pContext->GetConfig().SolutionHint)
    {
        pContext->GetStatistics().Increment(EventCounter::MovesWithHint);
    }

    UpdateDisplay(pContext);
}

void KPstateGame::Cheat1(KPstateContext *pContext) const
{
    KPboard board;

    board.InitializeToken(TokenId::GREEN1, 0, 0);
    board.InitializeToken(TokenId::GREEN2, 3, 3);
    board.InitializeToken(TokenId::GREEN3, 3, 4);
    board.InitializeToken(TokenId::GREEN4, 1, 2);
    board.InitializeToken(TokenId::WHITE1, 0, 1);
    board.InitializeToken(TokenId::WHITE2, 1, 0);
    board.InitializeToken(TokenId::WHITE3, 2, 0);
    board.InitializeToken(TokenId::WHITE4, 3, 0);
    board.InitializeToken(TokenId::WHITE5, 2, 2);
    board.InitializeToken(TokenId::RED1,   0, 3);

    pContext->GetBoardView().SetBoard(board);
    pContext->GetStatistics().Increment(EventCounter::MovesWithCheat);

    UpdateDisplay(pContext);
}

void KPstateGame::SaveGameStatus(KPstateContext *pContext) const
{
    auto &config = pContext->GetConfig();
    KPStatistics &statistics = pContext->GetStatistics();

    config.SavedGame     = pContext->GetBoardView().GetBoardId();
    config.PlayTime      = statistics.GetTotalTime();
    config.Moves         = statistics.Get(EventCounter::Moves);
    config.MovesWithHint = statistics.Get(EventCounter::MovesWithHint);
    config.MovesWithCheat = statistics.Get(EventCounter::MovesWithCheat);
    config.WriteToFile();
}

void KPstateGame::PlayAudioForInitialize(KPstateContext *pContext) const
{
    pContext->GetUserInterface().PlayAudio(KPSound::OpenGame);
}
