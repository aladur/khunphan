/*
    kpmenu.cpp

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

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <GL/glu.h>
#include <GL/gl.h>
#include <string>
#include "misc1.h"
#include "kpmenu.h"
#include "kplocale.h"
#include "kpconfig.h"
#include "sprinter.h"
#include "kpstate.h"


KPmenu::KPmenu() : IsDisplayOpenGLInfo(false), lastState(KPState_Invalid)
{
}

bool KPmenu::Initialize(std::string &TextureName, int TextureSize, bool Nearest,
                        int Language /* = 0*/)
{
    plates[PLATE_MENUBACKGROUND] = Plate(0.7f, 0.7f, 0.7f);
    plates[PLATE_SHADER] = Plate(0.0, 0.0, 0.0);
    plates[PLATE_LOGO] = Plate();
    plates[PLATE_SOUND_ON] = Plate();
    plates[PLATE_SOUND_OFF] = Plate();
    plates[PLATE_MUSIC_ON] = Plate();
    plates[PLATE_MUSIC_OFF] = Plate();

    Update(TextureName, TextureSize, Nearest);

    LOG1("Loading languages");
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
        message(mtError, " *** Error loading language. Program aborted\n");
        return false;
    }

    LOG1("Generating Menu Display Lists");

    tIdToLabel::iterator it;

    for (it = labels.begin(); it != labels.end(); ++it)
    {
        it->second.RecreateDisplayList();
    }

    progressBar.Initialize();

    return true;
}

void KPmenu::Update(std::string &TextureName, int TextureSize, bool Nearest,
                    bool always)
{
    //Create texture for labels
    Label::PreInitialize(TextureName, TextureSize, Nearest,
                         &KPConfig::Instance(), always);

    plates[PLATE_LOGO].Update(TextureName, TextureSize, Nearest,
                      true, "logo", &KPConfig::Instance(), always);
    plates[PLATE_SOUND_ON].Update(TextureName, TextureSize, Nearest,
                      true, "sound_on", &KPConfig::Instance(), always);
    plates[PLATE_SOUND_OFF].Update(TextureName, TextureSize, Nearest,
                      true, "soundmusic_off", &KPConfig::Instance(), always);
    plates[PLATE_MUSIC_ON].Update(TextureName, TextureSize, Nearest,
                      true, "music_on", &KPConfig::Instance(), always);
    plates[PLATE_MUSIC_OFF].Update(TextureName, TextureSize, Nearest,
                      true, "soundmusic_off", &KPConfig::Instance(), always);
}

bool KPmenu::LoadLanguage(int Language)
{
    std::string file;

    if (labels.find(Language) != labels.end())
    {
        LOG2(" ", labels[Language].GetText().c_str());
    }

    sprinter::sprintf(file,
                      "%s%d.lang",
                      KPConfig::Instance().GetDirectory(KP_LOCALE_DIR).c_str(),
                      Language);

    KPlocale locale(file);
    tIdToString strings = locale.GetStrings();

    if (strings.size() == 0)
    {
        return false;
    }

    tIdToString::iterator it;

    for (it = strings.begin(); it != strings.end(); ++it)
    {
        AddOrSetLabel(it->first, it->second);
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
    tIdToPlate::iterator it;

    for (it = plates.begin(); it != plates.end(); ++it)
    {
        it->second.SetFadeOut();
    }
}

void KPmenu::FadeOutAllLabels()
{
    Label::FadeOutAll();
}

void KPmenu::Draw()
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0,16.0,0.0,12.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    tIdToPlate::iterator sit;

    for (sit = plates.begin(); sit != plates.end(); ++sit)
    {
        if (sit->first != PLATE_SHADER)
        {
            sit->second.Draw();
        }
    }

    tIdToLabel::iterator tit;

    for (tit = labels.begin(); tit != labels.end(); ++tit)
    {
        tit->second.Draw();
    }

    glDisable(GL_TEXTURE_2D);

    progressBar.Draw();

    // This should always be the last one because it
    // should shade all other drawing primitives
    plates[PLATE_SHADER].Draw();

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
}

void KPmenu::UpdateFPS(int fps, float renderTime)
{
    labels[T_FPS].FormatText(fps, renderTime);
}
