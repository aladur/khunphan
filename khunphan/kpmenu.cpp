/*
    KPmenu.cpp

    Automatic solution finder for KhunPhan game
    Copyright (C) 2001,2002,2003  W. Schwotzer

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


KPmenu::KPmenu() : IsDisplayOpenGLInfo(false), lastState(0)
{
}

KPmenu::~KPmenu()
{
  tTextfeldArray::iterator tit;
  tSchildArray::iterator sit;

  for (tit = TextfeldArray.begin(); tit != TextfeldArray.end(); tit++ )
    delete tit->second;
  for (sit = SchildArray.begin(); sit != SchildArray.end(); sit++ )
    delete sit->second;
}

bool KPmenu::Initialize(const char *TextureName, int TextureSize, bool Nearest, int Language /* = 0*/)
{
  SchildArray[SHLD_MENUBACKGROUND] = new Schild;
  SchildArray[SHLD_SHADER        ] = new Schild;
  SchildArray[SHLD_LOGO          ] = new Schild;
  SchildArray[SHLD_SOUND_ON      ] = new Schild;
  SchildArray[SHLD_SOUND_OFF     ] = new Schild;
  SchildArray[SHLD_MUSIC_ON      ] = new Schild;
  SchildArray[SHLD_MUSIC_OFF     ] = new Schild;
  SchildArray[SHLD_MENUBACKGROUND]->Initialisiere(0.7f,0.7f,0.7f);
  SchildArray[SHLD_SHADER        ]->Initialisiere(0.0, 0.0, 0.0);

  Update(TextureName, TextureSize, Nearest);

  DEBUGPRINT("Loading languages\n");
  LoadLanguage(T_LANGUAGE_MAX);
  LoadLanguage(T_LANGUAGE1);
  LoadLanguage(T_LANGUAGE1 + 1);

  if (Language)
    LoadLanguage(Language);

  if (TextfeldArray.empty()) {
      message(mtError, " *** Error loading language. Program aborted\n");
      return false;
  }

  DEBUGPRINT("Generating Menu Display Lists\n");

  tTextfeldArray::iterator it;  
  //int i=0;

  for (it = TextfeldArray.begin(); it != TextfeldArray.end(); it++ ) {
    it->second->GeneriereDisplayList();
    //if ((++i % 10) == 0) { printf("."); fflush(stdout); }
  }

  TextfeldArray[T_FPS]->FormatText(NULL, 0);

  progressBar.Initialize();

  return true;
}

void KPmenu::Update(const char *TextureName, int TextureSize, bool Nearest, bool always)
{
  //Create texture for TextFeldArray
  Textfeld::PreInitialize(TextureName, TextureSize, Nearest, &KPConfig::Instance(), always);

  SchildArray[SHLD_LOGO     ]->Initialisiere(TextureName, TextureSize, Nearest, true, "logo",           &KPConfig::Instance(), always);
  SchildArray[SHLD_SOUND_ON ]->Initialisiere(TextureName, TextureSize, Nearest, true, "sound_on",       &KPConfig::Instance(), always);
  SchildArray[SHLD_SOUND_OFF]->Initialisiere(TextureName, TextureSize, Nearest, true, "soundmusic_off", &KPConfig::Instance(), always);
  SchildArray[SHLD_MUSIC_ON ]->Initialisiere(TextureName, TextureSize, Nearest, true, "music_on",       &KPConfig::Instance(), always);
  SchildArray[SHLD_MUSIC_OFF]->Initialisiere(TextureName, TextureSize, Nearest, true, "soundmusic_off", &KPConfig::Instance(), always);
}

bool KPmenu::LoadLanguage(int Language)
{
  char wort[MAX_LINE];
  char temp[MAX_LINE];
  int  i;
  int  index;

  FILE *f;
  BString file;

  if (TextfeldArray.find(Language) != TextfeldArray.end())
    DEBUGPRINT1(" %s\n", TextfeldArray[Language]->Text());

  file.printf("%s%d.lang",(const char *)KPConfig::Instance().GetDirectory(KP_LOCALE_DIR), Language);

#ifdef WIN32
  f=fopen(file, "rb");
#endif
#ifdef LINUX
  f=fopen(file, "r");
#endif
  if (f) {
    GLint nummer;
    while (!feof(f)) {
      fgets(temp,MAX_LINE,f);
      i = strlen(temp);
      while (i && (temp[--i] == 0x0a || temp[i] == 0x0d || temp[i] == ' ' || temp[i] == '\t')) temp[i]=0;
      if (sscanf(temp,"%d %n", &nummer, &index) < 1) continue;

      strcpy(wort, &temp[index]);
      AddTextField(nummer, wort);
    }
    fclose(f);
    // Single source for Package Name and Version
    sprintf(wort, "%s V%s", PACKAGE, VERSION);
    AddTextField(0, wort);
    return true;
  }
  return false;
}

void KPmenu::AddTextField(int number, char word[])
{
  if (word != NULL) {
    if (TextfeldArray.find(number) != TextfeldArray.end()) {
      TextfeldArray[number]->SetzeTextKDL(word);
    } else {
      TextfeldArray[number] = new Textfeld();
      TextfeldArray[number]->InitialisiereKDL(word);
    }
  }
}

void KPmenu::DeactivateAllLabels()
{
  for (unsigned int i=0; i < SchildArray.size(); i++)
    SchildArray[i]->Desaktiviere();
}

void KPmenu::DeactivateAllTextFields()
{
  Textfeld::DeactivateAll();
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

  tSchildArray::iterator sit;

  for (sit = SchildArray.begin(); sit != SchildArray.end(); sit++ )
    if (sit->second != SchildArray[SHLD_SHADER])
	  sit->second->male();

  tTextfeldArray::iterator tit;

  for (tit = TextfeldArray.begin(); tit != TextfeldArray.end(); tit++ )
    tit->second->male();

  glDisable(GL_TEXTURE_2D);

  progressBar.Draw();

  // This should always be the last one because it
  // should shade all other drawing primitives
  SchildArray[SHLD_SHADER]->male();
  
  glDisable(GL_BLEND);
  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);
}

void KPmenu::UpdateFPS(int fps, float renderTime)
{
  TextfeldArray[T_FPS]->FormatText(NULL, fps, (float)renderTime);
}
