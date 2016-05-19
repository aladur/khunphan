/*
    signals.h

    Automatic solution finder for KhunPhan game
    Copyright (C) 2016  W. Schwotzer

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

#ifndef SIGNALS_H
#define SIGNALS_H


enum class Signal
{
    Void = 0,

    MainMenu = 100,
    Back = 101,
    Continue = 102,
    ContinueGame = 103,

    NewGame = 110,
    Tutorial = 111,
    Settings = 112,
    Quit = 113,
    Language = 114,
    Graphics = 115,
    Control = 116,
    Audio = 117,
    KeyboardHelp = 118,
    ScoreList = 119,

    ToggleIllumination = 130,
    ToggleTextureName = 131,
    ToggleResolution = 132,
    ToggleTextureInterpolation = 133,
    ToggleMenuTextures = 134,
    ToggleTextures = 135,
    ToggleQuality = 136,
    ToggleFramerate = 137,
    ToggleSolutionHint = 138,
    ToggleMouseSpeed = 139,
    ToggleAmbientLight = 140,
    ToggleScreenmode = 141,
    ToggleSoundVolume = 142,
    ToggleMusicVolume = 143,
    ToggleUserInterface = 144,
    ToggleSoundOn = 145,
    ToggleMusicOn = 146,

    SelectEnglish = 900,
    SelectGerman = 901,
    // More languages may be available. No entry in this list is
    // needed. They are automatically detected from file 999.lang

    LanguageList = 999,
};

#endif
