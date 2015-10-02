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
#include <stdio.h>
#include "misc1.h"
#include "kpmenu.h"
#include "kpconfig.h"
#include "sprinter.h"
#include "kpstate.h"


KPmenu::KPmenu() : IsDisplayOpenGLInfo(false), lastState(KPState_Invalid)
{
}

bool KPmenu::Initialize(const char *TextureName, int TextureSize, bool Nearest,
                        int Language /* = 0*/)
{
    plates[PLATE_MENUBACKGROUND] = Plate();
    plates[PLATE_SHADER] = Plate();
    plates[PLATE_LOGO] = Plate();
    plates[PLATE_SOUND_ON] = Plate();
    plates[PLATE_SOUND_OFF] = Plate();
    plates[PLATE_MUSIC_ON] = Plate();
    plates[PLATE_MUSIC_OFF] = Plate();

    plates[PLATE_MENUBACKGROUND].Initialize(0.7f, 0.7f, 0.7f);
    plates[PLATE_SHADER].Initialize(0.0, 0.0, 0.0);

    Update(TextureName, TextureSize, Nearest);

    LOG1("Loading languages");
    LoadLanguage(T_LANGUAGE_MAX);
    LoadLanguage(T_LANGUAGE1);
    LoadLanguage(T_LANGUAGE1 + 1);

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

void KPmenu::Update(const char *TextureName, int TextureSize, bool Nearest,
                    bool always)
{
    //Create texture for TextFeldArray
    Label::PreInitialize(TextureName, TextureSize, Nearest,
                         &KPConfig::Instance(), always);

    plates[PLATE_LOGO].Initialize(TextureName, TextureSize, Nearest,
                      true, "logo", &KPConfig::Instance(), always);
    plates[PLATE_SOUND_ON].Initialize(TextureName, TextureSize, Nearest,
                      true, "sound_on", &KPConfig::Instance(), always);
    plates[PLATE_SOUND_OFF].Initialize(TextureName, TextureSize, Nearest,
                      true, "soundmusic_off", &KPConfig::Instance(), always);
    plates[PLATE_MUSIC_ON].Initialize(TextureName, TextureSize, Nearest,
                      true, "music_on", &KPConfig::Instance(), always);
    plates[PLATE_MUSIC_OFF].Initialize(TextureName, TextureSize, Nearest,
                      true, "soundmusic_off", &KPConfig::Instance(), always);
}

bool KPmenu::LoadLanguage(int Language)
{
    char temp[MAX_LINE];
    int  index;

    FILE *f = NULL;
    std::string file;

    if (labels.find(Language) != labels.end())
    {
        LOG2(" ", labels[Language].GetText().c_str());
    }
    sprinter::sprintf(file,
                      "%s%d.lang",
                      KPConfig::Instance().GetDirectory(KP_LOCALE_DIR).c_str(),
                      Language);
#ifdef WIN32
    f=fopen(file.c_str(), "rb");
#endif
#ifdef LINUX
    f=fopen(file.c_str(), "r");
#endif
    if (f)
    {
        char wort[MAX_LINE];

        GLint nummer;
        while (!feof(f))
        {
            int  i;

            fgets(temp,MAX_LINE,f);
            i = strlen(temp);
            while (i && (temp[--i] == 0x0a || temp[i] == 0x0d ||
                         temp[i] == ' '    || temp[i] == '\t'))
            {
                temp[i]=0;
            }
            if (sscanf(temp,"%d %n", &nummer, &index) < 1)
            {
                continue;
            }

            strcpy(wort, &temp[index]);
            AddTextField(nummer, wort);
        }
        fclose(f);
        // Single source for Package Name and Version
        std::stringstream text;
        text << PACKAGE << " V" << VERSION;
        AddTextField(0, text.str().c_str());
        return true;
    }
    return false;
}

void KPmenu::AddTextField(int number, const char word[])
{
    if (word != NULL)
    {
        if (labels.find(number) != labels.end())
        {
            labels[number].SetTextNDL(word);
        }
        else
        {
            labels[number] = Label();
            labels[number].Initialize(word);
        }
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
