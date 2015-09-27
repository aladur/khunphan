/*
    kpstateaudiosettings.cpp

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

#include "kpstateaudiosettings.h"
#include "kpmenu.h"
#include "kpconfig.h"
#include "kpuibase.h"

void KPstateAudioSettings::Initialize(KPstateContext *pContext,
                                      const KPstate *pOldState)
{
    KPstate::Initialize(pContext, pOldState);

    // Do some initialization stuff here:

    pContext->GetUserInterface().PlayMusic(true, true);
    E_MusicVolume = KPConfig::Instance().MusicVolume;
    E_SoundVolume = KPConfig::Instance().SoundVolume;

    UpdateDisplay(pContext);
}

void KPstateAudioSettings::UpdateDisplay(KPstateContext *pContext)
{
    KPstate::UpdateDisplay(pContext);

    KPmenu &menu = pContext->GetMenu();

    float y;
    float dy = 0.6f;
    int textfeldIdx;

    menu.plates[SHLD_MENUBACKGROUND].Positioniere(2,3.5,14,8.5);

    menu.plates[SHLD_LOGO].Positioniere(4,9,12,11);
    menu.plates[SHLD_LOGO].VollSichtbar();

    menu.labels[T_SETTINGS].Positioniere(4,8,1);
    menu.labels[T_SETTINGS].VollSichtbar();
    menu.labels[T_AUDIO].Positioniere(12,8,1,A_RIGHT);
    menu.labels[T_AUDIO].VollSichtbar();

    y = 6.0;
    menu.labels[T_SOUND_VOLUME].Positioniere(8,y,0.71f,A_RIGHT);
    menu.labels[T_SOUND_VOLUME].SetzeSignal(S_TOGGLE_SOUND_VOLUME);

    if (E_SoundVolume >= 100)
    {
        textfeldIdx = T_SND_VOL_100;
    }
    else if (E_SoundVolume >= 90)
    {
        textfeldIdx = T_SND_VOL_90;
    }
    else if (E_SoundVolume >= 80)
    {
        textfeldIdx = T_SND_VOL_80;
    }
    else if (E_SoundVolume >= 70)
    {
        textfeldIdx = T_SND_VOL_70;
    }
    else if (E_SoundVolume >= 60)
    {
        textfeldIdx = T_SND_VOL_60;
    }
    else if (E_SoundVolume >= 50)
    {
        textfeldIdx = T_SND_VOL_50;
    }
    else if (E_SoundVolume >= 40)
    {
        textfeldIdx = T_SND_VOL_40;
    }
    else if (E_SoundVolume >= 30)
    {
        textfeldIdx = T_SND_VOL_30;
    }
    else if (E_SoundVolume >= 20)
    {
        textfeldIdx = T_SND_VOL_20;
    }
    else if (E_SoundVolume >= 10)
    {
        textfeldIdx = T_SND_VOL_10;
    }
    else
    {
        textfeldIdx = T_SND_VOL_OFF;
    }

    menu.labels[textfeldIdx].Positioniere(8.2f,y,0.71f);
    menu.labels[textfeldIdx].SetzeSignal(S_TOGGLE_SOUND_VOLUME);

    y -= dy;
    menu.labels[T_MUSIC_VOLUME].Positioniere(8,y,0.71f,A_RIGHT);
    menu.labels[T_MUSIC_VOLUME].SetzeSignal(S_TOGGLE_MUSIC_VOLUME);

    if (E_MusicVolume >= 100)
    {
        textfeldIdx = T_MSC_VOL_100;
    }
    else if (E_MusicVolume >= 90)
    {
        textfeldIdx = T_MSC_VOL_90;
    }
    else if (E_MusicVolume >= 80)
    {
        textfeldIdx = T_MSC_VOL_80;
    }
    else if (E_MusicVolume >= 70)
    {
        textfeldIdx = T_MSC_VOL_70;
    }
    else if (E_MusicVolume >= 60)
    {
        textfeldIdx = T_MSC_VOL_60;
    }
    else if (E_MusicVolume >= 50)
    {
        textfeldIdx = T_MSC_VOL_50;
    }
    else if (E_MusicVolume >= 40)
    {
        textfeldIdx = T_MSC_VOL_40;
    }
    else if (E_MusicVolume >= 30)
    {
        textfeldIdx = T_MSC_VOL_30;
    }
    else if (E_MusicVolume >= 20)
    {
        textfeldIdx = T_MSC_VOL_20;
    }
    else if (E_MusicVolume >= 10)
    {
        textfeldIdx = T_MSC_VOL_10;
    }
    else
    {
        textfeldIdx = T_MSC_VOL_OFF;
    }

    menu.labels[textfeldIdx].Positioniere(8.2f,y,0.71f);
    menu.labels[textfeldIdx].SetzeSignal(S_TOGGLE_MUSIC_VOLUME);

    menu.labels[T_BACK].Positioniere(8,0.7f,1,A_CENTERED);
    menu.labels[T_BACK].SetzeSignal(S_BACK);

    StartAnimation();
}

void KPstateAudioSettings::KeyPressed (KPstateContext *pContext,
                                       unsigned char key, int x, int y)
{
    CHECK_DEFAULT_KEY_PRESSED(pContext, key, x, y);
}

void  KPstateAudioSettings::MouseClick (KPstateContext *pContext,
                                        tMouseButton button, tMouseEvent event,
                                        int x, int y)
{
    int Signal = KPstate::EvaluateMouseClick(pContext, button, event, x, y);

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
            pContext->ChangeState(oldStateId);
    }
}

tKPMenuState KPstateAudioSettings::ESCKeyAction (KPstateContext *pContext)
{
    SaveChanges(pContext);
    return oldStateId;
}

void KPstateAudioSettings::SaveChanges(KPstateContext *pContext)
{
    KPConfig::Instance().MusicVolume = E_MusicVolume;
    KPConfig::Instance().SoundVolume = E_SoundVolume;

    KPConfig::Instance().WriteToFile();

    pContext->GetUserInterface().PlayMusic(false);
}

void KPstateAudioSettings::ToggleMusicVolume(KPstateContext *pContext)
{
    pContext->GetUserInterface().PlayAudio(KP_SND_CHANGESETTING);
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
    pContext->GetUserInterface().PlayAudio(KP_SND_CHANGESETTING);
}
