/*
    kpstateaudiosettings.cpp

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
#include "kpstateaudiosettings.h"
#include "kpmenu.h"
#include "kpconfig.h"
#include "kpuibase.h"

KPstateAudioSettings::KPstateAudioSettings() :
    E_MusicVolume(0), E_SoundVolume(0)
{
}

void KPstateAudioSettings::Initialize(KPstateContext *pContext,
                                      StateId previousStateId)
{
    KPstate::Initialize(pContext, previousStateId);

    pContext->GetUserInterface().PlayMusic(true, true);
    E_MusicVolume = pContext->GetConfig().MusicVolume;
    E_SoundVolume = pContext->GetConfig().SoundVolume;

    UpdateDisplay(pContext);
}

void KPstateAudioSettings::UpdateDisplay(KPstateContext *pContext) const
{
    KPstate::UpdateDisplay(pContext);

    auto &menu = pContext->GetMenu();

    const auto dy = 0.6f;
    static const Lbl soundVolumeIds[11] =
    {
        Lbl::SoundVolumeOff, Lbl::SoundVolume10, Lbl::SoundVolume20,
        Lbl::SoundVolume30, Lbl::SoundVolume40, Lbl::SoundVolume50,
        Lbl::SoundVolume60, Lbl::SoundVolume70, Lbl::SoundVolume80,
        Lbl::SoundVolume90, Lbl::SoundVolume100
    };
    static const Lbl musicVolumeIds[11] =
    {
        Lbl::MusicVolumeOff, Lbl::MusicVolume10, Lbl::MusicVolume20,
        Lbl::MusicVolume30, Lbl::MusicVolume40, Lbl::MusicVolume50,
        Lbl::MusicVolume60, Lbl::MusicVolume70, Lbl::MusicVolume80,
        Lbl::MusicVolume90, Lbl::MusicVolume100
    };

    menu.plates[KPPlate::MenuBackground].SetPosition(2, 3.5, 14, 8.5);

    menu.plates[KPPlate::Logo].SetPosition(4, 9, 12, 11);
    menu.plates[KPPlate::Logo].SetFullyVisible();

    menu.labels[Lbl::Settings].SetPosition(4, 8, 1);
    menu.labels[Lbl::Settings].SetFullyVisible();
    menu.labels[Lbl::Audio].SetPosition(12, 8, 1, AlignItem::Right);
    menu.labels[Lbl::Audio].SetFullyVisible();

    auto y = 6.0f;
    menu.labels[Lbl::SoundVolume].SetPosition(8, y, 0.71f, AlignItem::Right);
    menu.labels[Lbl::SoundVolume].SetSignal(S_TOGGLE_SOUND_VOLUME);

    auto labelId = soundVolumeIds[E_SoundVolume / 10];

    menu.labels[labelId].SetPosition(8.2f, y, 0.71f);
    menu.labels[labelId].SetSignal(S_TOGGLE_SOUND_VOLUME);

    y -= dy;
    menu.labels[Lbl::MusicVolume].SetPosition(8, y, 0.71f, AlignItem::Right);
    menu.labels[Lbl::MusicVolume].SetSignal(S_TOGGLE_MUSIC_VOLUME);

    labelId = musicVolumeIds[E_MusicVolume / 10];

    menu.labels[labelId].SetPosition(8.2f, y, 0.71f);
    menu.labels[labelId].SetSignal(S_TOGGLE_MUSIC_VOLUME);

    menu.labels[Lbl::Back].SetPosition(8, 0.7f, 1, AlignItem::Centered);
    menu.labels[Lbl::Back].SetSignal(S_BACK);

    StartAnimation(pContext);
}

void  KPstateAudioSettings::MouseClick(KPstateContext *pContext,
                                       MouseButton button,
                                       MouseButtonEvent event,
                                       int x, int y)
{
    auto Signal = KPstate::EvaluateMouseClick(pContext, button, event, x, y);

    switch (Signal)
    {
        case S_TOGGLE_SOUND_VOLUME:
            ToggleSoundVolume(pContext);
            UpdateDisplay(pContext);
            break;

        case S_TOGGLE_MUSIC_VOLUME:
            ToggleMusicVolume(pContext);
            UpdateDisplay(pContext);
            break;

        case S_BACK:
            SaveChanges(pContext);
            pContext->ChangeState(pContext->GetPreviousState());
    }
}

StateId KPstateAudioSettings::ESCKeyAction(KPstateContext *pContext) const
{
    SaveChanges(pContext);
    return pContext->GetPreviousState();
}

void KPstateAudioSettings::SaveChanges(KPstateContext *pContext) const
{
    pContext->GetConfig().MusicVolume = E_MusicVolume;
    pContext->GetConfig().SoundVolume = E_SoundVolume;

    pContext->GetConfig().WriteToFile();

    pContext->GetUserInterface().PlayMusic(false);
}

void KPstateAudioSettings::ToggleMusicVolume(KPstateContext *pContext)
{
    pContext->GetUserInterface().PlayAudio(KPSound::ChangeSetting);

    if (E_MusicVolume >= 100)
    {
        E_MusicVolume = 0;
    }
    else
    {
        E_MusicVolume += 10;
    }

    pContext->GetUserInterface().SetMusicVolume(E_MusicVolume);
}

void KPstateAudioSettings::ToggleSoundVolume(KPstateContext *pContext)
{
    if (E_SoundVolume >= 100)
    {
        E_SoundVolume = 0;
    }
    else
    {
        E_SoundVolume += 10;
    }

    pContext->GetUserInterface().SetSoundVolume(E_SoundVolume);
    // First change sound volume than play sound
    pContext->GetUserInterface().PlayAudio(KPSound::ChangeSetting);
}
