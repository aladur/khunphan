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

// The values defined in this enum are IDs to translated strings,
// and also used as index into the labels list.
// The language specific strings are defined in locale/9xy.lang

#ifndef Language_H
#define Language_H

enum class Lbl
{
    ApplicationVersion = 0,
    Copyright1 = 1,
    Copyright2 = 2,
    Copyright3 = 3,
    Copyright4 = 4,
    Copyright5 = 5,
    Copyright6 = 6,
    Copyright7 = 7,
    Copyright8 = 8,
    NoWarranty = 9,

    // Main menu
    MainMenu = 10,
    Settings = 11,
    KeyboardHelp = 12,
    Quit = 13,
    Tutorial = 14,
    Continue = 15,

    // Settings
    Control = 20,
    Graphics = 21,
    Audio = 22,
    Language = 23,

    // OpenGL interface
    OpenGLVendor = 30,
    OpenGLRenderer = 31,
    OpenGLVersion = 32,

    // Display Fps
    Fps = 35,

    // Keyboard Help
    HelpGeneral = 40,
    HelpInGame = 41,

    HelpKeyESC = 42,
    HelpKeyOpenGL = 43,
    HelpKeyPause = 44,
    HelpKeyCamera = 45,
    HelpKeyFullscreen = 46,

    HelpESC = 50,
    HelpOpenGL = 51,
    HelpPause = 52,
    HelpCamera = 53,
    HelpFullscreen = 54,

    // General
    Back = 100,
    Store = 101,
    Ok = 102,
    Cancel = 103,
    Pause = 104,
    DoQuit = 105,

    // Game solved
    ContinueGame = 170,
    Solved = 171,

    Solved11 = 172,
    Solved12 = 173,
    Solved13 = 174,

    SolvedWithHint21 = 175,
    SolvedWithHint22 = 176,
    SolvedWithHint23 = 177,

    SolvedWithCheat31 = 178,
    SolvedWithCheat32 = 179,
    SolvedWithCheat33 = 180,

    InputName = 181,
    NameLabel = 182,

    // In the game
    NewGame = 192,
    Time = 193,
    MoveCountHint = 194,
    MoveCount = 195,

    // Settings
    MouseSpeed = 200,
    MouseSpeedSlow = 201,
    MouseSpeedMedium = 202,
    MouseSpeedFast = 203,
    MouseSpeedVeryFast = 204,

    SolutionHint = 205,
    SolutionHintOn = 206,
    SolutionHintOff = 207,

    MenuTextures = 230,
    MenuTexturesNormal = 231,
    MenuTexturesHigh = 232,

    Textures = 250,
    TexturesVeryLow = 251,
    TexturesLow = 252,
    TexturesNormal = 253,
    TexturesHigh = 254,

    Resolution = 270,
    Resolution640x480 = 271,
    Resolution800x600 = 272,
    Resolution1024x768 = 273,
    Resolution1280x960 = 274,
    Resolution1600x1200 = 275,
    Resolution1920x1440 = 276,

    TextureInterpolation = 295,
    TextureInterpolationOff = 296,
    TextureInterpolationOn = 297,

    Quality = 300,
    QualityVeryFast = 301,
    QualityFast = 302,
    QualityNormal = 303,
    QualityHigh = 304,
    QualityVeryHigh = 305,
    QualityUserDefined = 306,

    Hint = 310,
    Hint1 = 311,
    Hint2 = 312,
    Hint3 = 313,

    AmbientLight = 320,
    AmbientLightOn = 321,
    AmbientLightOff = 322,

    Illumination = 325,
    Illumination1 = 326,
    Illumination2 = 327,
    Illumination3 = 328,

    Framerate = 335,
    FramerateOn = 336,
    FramerateOff = 337,

    Screenmode = 341,
    ScreenModeFullscreen = 342,
    ScreenModeWindow = 343,

    UserInterface = 345,
    UserInterface0 = 346,
    UserInterface1 = 347,

    // Settings - Audio
    SoundVolume = 350,
    SoundVolumeOff = 351,
    SoundVolume10 = 352,
    SoundVolume20 = 353,
    SoundVolume30 = 354,
    SoundVolume40 = 355,
    SoundVolume50 = 356,
    SoundVolume60 = 357,
    SoundVolume70 = 358,
    SoundVolume80 = 359,
    SoundVolume90 = 360,
    SoundVolume100 = 361,

    MusicVolume = 370,
    MusicVolumeOff = 371,
    MusicVolume10 = 372,
    MusicVolume20 = 373,
    MusicVolume30 = 374,
    MusicVolume40 = 375,
    MusicVolume50 = 376,
    MusicVolume60 = 377,
    MusicVolume70 = 378,
    MusicVolume80 = 379,
    MusicVolume90 = 380,
    MusicVolume100 = 381,

    TextureName = 382,
    TextureName1 = 383,
    TextureName2 = 384,
    TextureName3 = 385,

    // Settings - language
    SelectLanguage = 400,
    SelectEnglish = 401,
    SelectGerman = 402,

    // Score List
    ScoreList = 500,
    ScoreListName = 501,
    ScoreListPlayTime = 502,
    ScoreListMoves = 503,

    ScoreListName1 = 510,
    ScoreListPlayTime1 = 520,
    ScoreListMoves1 = 530,

    // Tutorial
    Tutorial1 = 710,
    Tutorial11 = 711,
    Tutorial12 = 712,
    Tutorial13 = 713,
    Tutorial14 = 714,
    Tutorial15 = 715,
    Tutorial16 = 716,
    Tutorial17 = 717,
    Tutorial18 = 718,
    Tutorial19 = 719,

    Tutorial2 = 720,
    Tutorial21 = 721,
    Tutorial22 = 722,
    Tutorial23 = 723,
    Tutorial24 = 724,
    Tutorial25 = 725,

    Tutorial3 = 730,
    Tutorial3Continue = 731,
    Tutorial31 = 732,
    Tutorial32 = 733,
    Tutorial33 = 734,
    Tutorial34 = 735,
    Tutorial35 = 736,
    Tutorial36 = 737,

    Tutorial4 = 750,
    Tutorial4HaveFun = 751,
    Tutorial41 = 752,
    Tutorial42 = 753,
    Tutorial43 = 754,
    Tutorial44 = 755,
    Tutorial45 = 756,
    Tutorial46 = 757,

    TutorialQuiteGood = 770,
    TutorialNotMoveable = 771,
    TutorialMissedToken = 772,
    TutorialTryAgain = 773,
    TutorialMouseMove = 774,

    TutorialEnd = 799,

    English = 900,
    German = 901,
    LanguageList = 999
};

#endif

