/*
    kpconfig.h


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

#ifndef KPCONFIG_H
#define KPCONFIG_H

#include "stdafx.h"
#include <vector>
#include <string>


enum tKPDir
{
    KP_TEXTURE_DIR = 26,
    KP_SOUND_DIR   = 27,
    KP_MUSIC_DIR   = 28,
    KP_LOCALE_DIR  = 29
};

class KPConfig
{
public:
    KPConfig();
    virtual ~KPConfig();
    static const char *GetUserInterfaceName(int ui);

    void SetDefaultValues();
    void ReadCommandLineParams(int argc, char **argv);
    void ReadFromFile();
    void WriteToFile() const;
    void DebugPrint() const;
    std::string GetFileName() const;
    std::string GetFileVersion() const;
    std::string GetDirectory(tKPDir directoryID) const;
    std::vector<std::string> GetTextureNames() const;
    void SetFileName(const char *fileName);

    bool FullScreen;
    bool Nearest;
    bool AmbientLight;
    bool Reflections;
    bool Shadows;
    bool DisplayFPS;
    bool SoundOn;
    bool MusicOn;

    int ScreenXResolution;
    int ColorDepth;
    int TextureSize;
    int MenuTextureSize;
    int LightSources;
    int Language;
    int UserInterface;
    int CameraPosition;

    int   SolutionHint;
    float MouseSpeed;

    int SoundVolume;
    int MusicVolume;

    QWord     SavedGame;
    unsigned int PlayTime;
    unsigned int Moves;
    unsigned int MovesWithHint;
    unsigned int CheatCount;

    std::string TextureName;

    bool DisplayVersionOnly;

    // only for debugging:
    bool PerformanceLog;
    bool SkipProgressBar;

protected:
    std::string GetDirectoryBase(tKPDir directoryID) const;

    std::string fileName;
    std::string fileVersion;
};

#endif
