/*
    kpstates.h

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

#ifndef KPSTATES_H
#define KPSTATES_H

#include "stdafx.h"


enum tKPMenuState
{
    KPState_Invalid         = 0,
    KPState_StartUp         = 1,
    KPState_Game            = 2,
    KPState_SelectLanguage  = 3,
    KPState_MainMenu        = 4,
    KPState_Finish          = 5,
    KPState_Settings        = 6,
    KPState_ControlSettings = 7,
    KPState_GraphicSettings = 8,
    KPState_GraphicHint     = 9,
    KPState_AudioSettings   = 10,
    KPState_Help            = 11,
    KPState_KeyboardHelp    = 12,
    KPState_GameSolved      = 13,
    KPState_Tutorial1       = 14,
    KPState_Tutorial2       = 15,
    KPState_Tutorial3       = 16,
    KPState_Tutorial4       = 17,
    KPState_ScoreList       = 18,
    KPState_LightTest       = 19
};

#endif
