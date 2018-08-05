/*
    kpstates.h

    Automatic solution finder for KhunPhan game
    Copyright (C) 2016-2018  W. Schwotzer

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

#ifndef KPSTATES_H
#define KPSTATES_H


enum class StateId
{
    Invalid,
    StartUp,
    Game,
    SelectLanguage,
    MainMenu,
    Finish,
    Settings,
    ControlSettings,
    GraphicSettings,
    GraphicHint,
    AudioSettings,
    KeyboardHelp,
    GameSolved,
    Tutorial1,
    Tutorial2,
    Tutorial3,
    Tutorial4,
    ScoreList,
    LightTest
};

#endif
