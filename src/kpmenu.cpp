/*
    kpmenu.cpp

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
#include <GL/glu.h>
#include <GL/gl.h>
#include <string>
#include <stdexcept>
#include "kpmenu.h"
#include "kplocale.h"
#include "kpconfig.h"
#include "kpstate.h"
#include "kpstatistics.h"
#include "blogger.h"


KPmenu::KPmenu(KPConfigPtr PConfig) :
    lastState(KPState_Invalid),
    playTimeUpdateTimer(100, true, true), config(PConfig),
    isDisplayOpenGLInfo(false)
{
}

void KPmenu::Initialize(std::string &TextureName, int TextureSize, bool Nearest,
                        int Language /* = 0*/)
{
    BLogger::Log("Menu initialization");

    plates[PLATE_MENUBACKGROUND] = Plate(0.7f, 0.7f, 0.7f);
    plates[PLATE_SHADER] = Plate(0.0, 0.0, 0.0);
    plates[PLATE_LOGO] = Plate();
    plates[PLATE_SOUND_ON] = Plate();
    plates[PLATE_SOUND_OFF] = Plate();
    plates[PLATE_MUSIC_ON] = Plate();
    plates[PLATE_MUSIC_OFF] = Plate();

    Update(TextureName, TextureSize, Nearest);

    BLogger::Log("Loading languages");
    // In the first step load strings for all supported languages
    LoadLanguage(T_LANGUAGE_MAX);
    // In a second step load english as default
    LoadLanguage(T_LANGUAGE2);

    if (Language)
    {
        LoadLanguage(Language);
    }

    if (labels.empty())
    {
        throw std::runtime_error("Error loading language.");
    }
}

void KPmenu::Update(std::string &TextureName, int TextureSize, bool Nearest)
{
    //Create texture for labels
    Label::PreInitialize(TextureName, TextureSize, Nearest, *config);

    plates[PLATE_LOGO].Update(TextureName, TextureSize, Nearest,
                              true, "logo", *config);
    plates[PLATE_SOUND_ON].Update(TextureName, TextureSize, Nearest,
                                  true, "sound_on", *config);
    plates[PLATE_SOUND_OFF].Update(TextureName, TextureSize, Nearest,
                                   true, "soundmusic_off", *config);
    plates[PLATE_MUSIC_ON].Update(TextureName, TextureSize, Nearest,
                                  true, "music_on", *config);
    plates[PLATE_MUSIC_OFF].Update(TextureName, TextureSize, Nearest,
                                   true, "soundmusic_off", *config);
}

bool KPmenu::LoadLanguage(int Language)
{
    std::stringstream stream;
    std::string file;

    if (labels.find(Language) != labels.end())
    {
        BLogger::Log(" ", labels[Language].GetText().c_str());
    }

    stream << config->Get(KPDirectory::Locale).c_str()
           << Language
           << ".lang";

    file = stream.str();
    tIdToString strings = KPlocale::ReadFromFile(file);

    if (strings.size() == 0)
    {
        return false;
    }

    for (auto &item : strings)
    {
        AddOrSetLabel(item.first, item.second);
    }

    // Single source for Package Name and Version
    std::stringstream text;
    text << PACKAGE << " V" << VERSION;
    AddOrSetLabel(0, text.str());

    return true;
}

void KPmenu::AddOrSetLabel(int number, const std::string &text)
{
    if (labels.find(number) != labels.end())
    {
        labels[number].SetTextOrFormat(text);
    }
    else
    {
        labels[number] = Label(text);
    }
}

void KPmenu::FadeOutAllPlates()
{
    for (auto &plate : plates)
    {
        plate.second.SetFadeOut();
    }
}

void KPmenu::FadeOutAllLabels()
{
    Label::FadeOutAll();
}

void KPmenu::Animate(unsigned int duration)
{
    for (auto &item : plates)
    {
        item.second.Animate(duration);
    }

    for (auto &item : labels)
    {
        item.second.Animate(duration);
    }
}

void KPmenu::Draw()
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 16.0, 0.0, 12.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    for (const auto &item : plates)
    {
        if (item.first != PLATE_SHADER)
        {
            item.second.Draw();
        }
    }

    for (auto &item : labels)
    {
        item.second.Draw();
    }

    glDisable(GL_TEXTURE_2D);

    // This should always be the last one because it
    // should shade all other drawing primitives
    plates[PLATE_SHADER].Draw();

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
}

void KPmenu::UpdateFPS(int fps, float renderTime)
{
    std::stringstream stream;

    stream << std::fixed << std::setprecision(1) << renderTime;
    labels[T_FPS].FormatText(fps, stream.str());
}

void KPmenu::UpdatePlayTime(KPstateContext *pContext, unsigned int duration)
{
    if (playTimeUpdateTimer.Add(duration))
    {
        // Cyclically update play time each 100 ms
        std::string timeString =
            pContext->GetStatistics().GetTotalTime(TimeFormat::HHMMSS);
        labels[T_TIME].FormatText(timeString);
    }
}

