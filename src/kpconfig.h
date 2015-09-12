/*
    kpconfig.h


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

// The class KPConfig is implemented as Singleton and it can be accessed
// system wide with the static member function:
//
// KPConfig::Instance()
//
#ifndef KPCONFIG_H
#define KPCONFIG_H

#include <vector>
#include <string>
#include "misc1.h"


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
    static KPConfig &Instance();
    static void finalize()
    {
        delete instance;
        instance = NULL;
    };
    static const char *GetUserInterfaceName(int ui);

    void SetDefaultValues();
    void ReadCommandLineParams(int argc, char **argv);
    void ReadFromFile();
    void WriteToFile();
    void DebugPrint();
    std::string GetFileName();
    std::string GetFileVersion();
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

    // only for debugging:
    bool PerformanceLog;
    bool SkipProgressBar;

protected:
    KPConfig();
    std::string fileName;
    std::string fileVersion;

private:
    static KPConfig *instance;
    virtual ~KPConfig();
};

#endif
