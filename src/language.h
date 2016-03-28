/*
    language.h

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

// The values defined in this enum are IDs to translated strings.
// The language specific strings are defined in locale/xyz.lang

#ifndef LANGUAGE_H
#define LANGUAGE_H

enum tTxt
{
// Titel
    T_VERSION = 0,
    T_COPYRIGHT1 = 1,
    T_COPYRIGHT2 = 2,
    T_COPYRIGHT3 = 3,
    T_COPYRIGHT4 = 4,
    T_COPYRIGHT5 = 5,
    T_COPYRIGHT6 = 6,
    T_COPYRIGHT7 = 7,
    T_COPYRIGHT8 = 8,
    T_NOWARRANTY = 9,

// Main menu
    T_MAINMENU = 10,
    T_SETTINGS = 11,
    T_H_KEYBOARDHELP = 12,
    T_QUIT = 13,
    T_TUTORIAL = 14,
    T_CONTINUE = 15,

// Settings
    T_CONTROL = 20,
    T_GRAPHICS = 21,
    T_AUDIO = 22,
    T_LANGUAGE = 23,

// OpenGL interface
    T_GL_VENDOR = 30,
    T_GL_RENDERER = 31,
    T_GL_VERSION = 32,

// Display FPS
    T_FPS = 35,

// Keyboard Help
    T_H_GENERAL = 40,
    T_H_IN_GAME = 41,

    T_H_ESC_KEY = 42,
    T_H_OPENGL_KEY = 43,
    T_H_PAUSE_KEY = 44,
    T_H_CAMERA_KEY = 45,
    T_H_FULLSCREEN_KEY = 46,

    T_H_ESC = 50,
    T_H_OPENGL = 51,
    T_H_PAUSE = 52,
    T_H_CAMERA = 53,
    T_H_FULLSCREEN = 54,

// General
    T_BACK = 100,
    T_STORE = 101,
    T_OK = 102,
    T_CANCEL = 103,
    T_PAUSE = 104,
    T_DOQUIT = 105,

// Game solved
    T_CONTINUEGAME = 170,
    T_SOLVED1 = 171,

    T_SOLVED2_1 = 172,
    T_SOLVED3_1 = 173,
    T_SOLVED4_1 = 174,

    T_SOLVED2_2 = 175,
    T_SOLVED3_2 = 176,
    T_SOLVED4_2 = 177,

    T_SOLVED2_3 = 178,
    T_SOLVED3_3 = 179,
    T_SOLVED4_3 = 180,

    T_INPUT_NAME = 181,
    T_NAME_LABEL = 182,

// In the game
    T_NEWGAME = 192,
    T_TIME = 193,
    T_MINMOVECOUNT = 194,
    T_MOVECOUNT = 195,

// Settings
    T_MOUSESPEED = 200,
    T_MS_SLOW = 201,
    T_MS_MEDIUM = 202,
    T_MS_FAST = 203,
    T_MS_VERYFAST = 204,

    T_SOLUTIONHINT = 205,
    T_SH_ON = 206,
    T_SH_OFF = 207,

    T_MENUTEXTURES = 230,
    T_MT_NORMAL = 231,
    T_MT_HIGH = 232,

    T_TEXTURES = 250,
    T_T_VERYLOW = 251,
    T_T_LOW = 252,
    T_T_NORMAL = 253,
    T_T_HIGH = 254,

    T_RESOLUTION = 270,
    T_RES_640x480 = 271,
    T_RES_800x600 = 272,
    T_RES_1024x768 = 273,
    T_RES_1280x960 = 274,
    T_RES_1600x1200 = 275,
    T_RES_1920x1440 = 276,

    T_TEXTUREINTERPOL = 295,
    T_TI_OFF = 296,
    T_TI_ON = 297,

    T_QUALITY = 300,
    T_Q_VERYFAST = 301,
    T_Q_FAST = 302,
    T_Q_NORMAL = 303,
    T_Q_HIGH = 304,
    T_Q_VERYHIGH = 305,
    T_Q_USER = 306,

    T_HINT = 310,
    T_HINT1 = 311,
    T_HINT2 = 312,
    T_HINT3 = 313,

    T_AMBIENTLIGHT = 320,
    T_AL_ON = 321,
    T_AL_OFF = 322,

    T_ILLUMINATION = 325,
    T_ILLUM1 = 326,
    T_ILLUM2 = 327,
    T_ILLUM3 = 328,

    T_FRAMERATE = 335,
    T_FR_ON = 336,
    T_FR_OFF = 337,

    T_SCREENMODE = 341,
    T_FULLSCREEN = 342,
    T_WINDOW = 343,

    T_USERINTERFACE = 345,
    T_USERINTERFACE0 = 346,
    T_USERINTERFACE1 = 347,

// Settings - Audio
    T_SOUND_VOLUME = 350,
    T_SND_VOL_OFF = 351,
    T_SND_VOL_10 = 352,
    T_SND_VOL_20 = 353,
    T_SND_VOL_30 = 354,
    T_SND_VOL_40 = 355,
    T_SND_VOL_50 = 356,
    T_SND_VOL_60 = 357,
    T_SND_VOL_70 = 358,
    T_SND_VOL_80 = 359,
    T_SND_VOL_90 = 360,
    T_SND_VOL_100 = 361,

    T_MUSIC_VOLUME = 370,
    T_MSC_VOL_OFF = 371,
    T_MSC_VOL_10 = 372,
    T_MSC_VOL_20 = 373,
    T_MSC_VOL_30 = 374,
    T_MSC_VOL_40 = 375,
    T_MSC_VOL_50 = 376,
    T_MSC_VOL_60 = 377,
    T_MSC_VOL_70 = 378,
    T_MSC_VOL_80 = 379,
    T_MSC_VOL_90 = 380,
    T_MSC_VOL_100 = 381,

    T_TEXTURENAME = 382,
    T_TEXTURE1 = 383,
    T_TEXTURE2 = 384,
    T_TEXTURE3 = 385,

// Settings - language
    T_SELECTLANGUAGE = 400,
    T_NATIVE_LANG1 = 401,
    T_NATIVE_LANG2 = 402,

// Score List
    T_SCORELIST = 500,
    T_S_NAME = 501,
    T_S_PLAYTIME = 502,
    T_S_MOVES = 503,

    T_S_NAME1 = 510,
    T_S_PLAYTIME1 = 520,
    T_S_MOVES1 = 530,

// Tutorial
    T_TUTORIAL1 = 710,
    T_TUTORIAL1_01 = 711,
    T_TUTORIAL1_02 = 712,
    T_TUTORIAL1_03 = 713,
    T_TUTORIAL1_04 = 714,
    T_TUTORIAL1_05 = 715,
    T_TUTORIAL1_06 = 716,
    T_TUTORIAL1_07 = 717,
    T_TUTORIAL1_08 = 718,
    T_TUTORIAL1_09 = 719,

    T_TUTORIAL2 = 720,
    T_TUTORIAL2_01 = 721,
    T_TUTORIAL2_02 = 722,
    T_TUTORIAL2_03 = 723,
    T_TUTORIAL2_04 = 724,
    T_TUTORIAL2_05 = 725,

    T_TUTORIAL3 = 730,
    T_TUTORIAL3_GOON = 731,
    T_TUTORIAL3_01 = 732,
    T_TUTORIAL3_02 = 733,
    T_TUTORIAL3_03 = 734,
    T_TUTORIAL3_04 = 735,
    T_TUTORIAL3_05 = 736,
    T_TUTORIAL3_06 = 737,

    T_TUTORIAL4 = 750,
    T_TUTORIAL4_GOON = 751,
    T_TUTORIAL4_01 = 752,
    T_TUTORIAL4_02 = 753,
    T_TUTORIAL4_03 = 754,
    T_TUTORIAL4_04 = 755,
    T_TUTORIAL4_05 = 756,
    T_TUTORIAL4_06 = 757,

    T_TUTORIAL34_GOOD = 770,
    T_TUTORIAL34_NOMV = 771,
    T_TUTORIAL34_EMPTY = 772,
    T_TUTORIAL34_AGAIN = 773,
    T_TUTORIAL4_MOVE = 774,

    T_TUTORIAL_END = 799,

    T_LANGUAGE1 = 900,
    T_LANGUAGE2 = 901,
    T_LANGUAGE_MAX = 999
};

#endif

