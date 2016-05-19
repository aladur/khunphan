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
    lastState(StateId::Invalid),
    playTimeUpdateTimer(100, true, true), config(PConfig),
    isDisplayOpenGLInfo(false)
{
}

void KPmenu::Initialize(std::string &TextureName, int TextureSize, bool Nearest,
                        Signal Language)
{
    BLogger::Log("Menu initialization");

    plates[KPPlate::MenuBackground] = Plate(0.7f, 0.7f, 0.7f);
    plates[KPPlate::Shader] = Plate(0.0, 0.0, 0.0);
    plates[KPPlate::Logo] = Plate();
    plates[KPPlate::SoundOn] = Plate();
    plates[KPPlate::SoundOff] = Plate();
    plates[KPPlate::MusicOn] = Plate();
    plates[KPPlate::MusicOff] = Plate();

    Update(TextureName, TextureSize, Nearest);

    BLogger::Log("Loading languages");
    // In the first step load strings for all supported languages
    LoadLanguage(Signal::LanguageList);
    auto languageCount = labels.size();
    // In a second step load english as default
    LoadLanguage(Signal::SelectEnglish);
    // In a third step load the requested language
    LoadLanguage(Language);

    if (labels.size() <= languageCount)
    {
        throw std::runtime_error("Error loading language.");
    }
}

void KPmenu::Update(std::string &TextureName, int TextureSize, bool Nearest)
{
    //Create texture for labels
    Label::PreInitialize(TextureName, TextureSize, Nearest, *config);

    plates[KPPlate::Logo].Update(TextureName, TextureSize, Nearest,
                                 true, "logo", *config);
    plates[KPPlate::SoundOn].Update(TextureName, TextureSize, Nearest,
                                    true, "sound_on", *config);
    plates[KPPlate::SoundOff].Update(TextureName, TextureSize, Nearest,
                                     true, "soundmusic_off", *config);
    plates[KPPlate::MusicOn].Update(TextureName, TextureSize, Nearest,
                                    true, "music_on", *config);
    plates[KPPlate::MusicOff].Update(TextureName, TextureSize, Nearest,
                                     true, "soundmusic_off", *config);
}

bool KPmenu::LoadLanguage(Signal Language)
{
    std::stringstream stream;
    auto id = static_cast<std::size_t>(Language);
    auto labelId = static_cast<Lbl>(id);

    if (labels.find(labelId) != labels.end())
    {
        BLogger::Log(" ", labels[labelId].GetText());
    }

    stream << config->Get(KPDirectory::Locale) << id << KPlocale::FileExtension;

    auto file = stream.str();
    auto idsToStrings = KPlocale::ReadFromFile(file);

    if (idsToStrings.empty())
    {
        return false;
    }

    for (auto &item : idsToStrings)
    {
        AddOrSetLabel(item.first, item.second);
    }

    // Single source for Package Name and Version
    std::stringstream text;
    text << PACKAGE << " V" << VERSION;
    AddOrSetLabel(Lbl::ApplicationVersion, text.str());

    return true;
}

void KPmenu::AddOrSetLabel(Lbl labelId, const std::string &text)
{
    if (labels.find(labelId) != labels.end())
    {
        labels[labelId].SetTextOrFormat(text);
    }
    else
    {
        labels[labelId] = Label(text);
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
        if (item.first != KPPlate::Shader)
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
    plates[KPPlate::Shader].Draw();

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
}

void KPmenu::UpdateFPS(int fps, float renderTime)
{
    std::stringstream stream;

    stream << std::fixed << std::setprecision(1) << renderTime;
    labels[Lbl::Fps].FormatText(fps, stream.str());
}

void KPmenu::UpdatePlayTime(KPstateContext *pContext, unsigned int duration)
{
    if (playTimeUpdateTimer.Add(duration))
    {
        // Cyclically update play time each 100 ms
        std::string timeString =
            pContext->GetStatistics().GetTotalTime(TimeFormat::HHMMSS);
        labels[Lbl::Time].FormatText(timeString);
    }
}

