/*
    kpconfig.cpp


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


#include "stdafx.h"
#ifdef _WIN32
    #include <shlwapi.h>
    #pragma comment(lib,"shlwapi.lib")
    #include "shlobj.h"
#endif
#ifdef HAVE_UNISTD_H
    #include <sys/types.h>
    #include <unistd.h>
#endif
#ifdef HAVE_SYS_STAT_H
    #include <sys/types.h>
    #include <sys/stat.h>
#endif
#include <sstream>
#include <libxml/tree.h>
#include "kpconfig.h"
#include "bdir.h"
#include "blogger.h"

#define _TO(type)   reinterpret_cast<const xmlChar *>(type)
#define _FROM(type) reinterpret_cast<const char *>(type)


KPConfig::KPConfig() :
    FullScreen(false), Nearest(true), AmbientLight(false),
    Reflections(false), Shadows(false), DisplayFPS(false),
    SoundOn(true), MusicOn(true),
    ScreenXResolution(640), ColorDepth(32), TextureSize(2),
    MenuTextureSize(2), LightSources(2), Language(0),
    UserInterface(0), CameraPosition(2),
    SolutionHint(0), MouseSpeed(0.4f),
    SoundVolume(100), MusicVolume(100),
    SavedGame(0), PlayTime(0), Moves(0), MovesWithHint(0),
    CheatCount(0),
    TextureName("wood"), DisplayVersionOnly(false),
    PerformanceLog(false), SkipProgressBar(false)
{
    SetFileName(nullptr);
}

KPConfig::~KPConfig()
{
    xmlCleanupParser();
}

std::string KPConfig::GetFileName() const
{
    return fileName;
}

std::string KPConfig::GetFileVersion() const
{
    return fileVersion;
}

// if aFileName is nullptr initialize it to the default
void KPConfig::SetFileName(const char *aFileName)
{
    if (aFileName != nullptr)
    {
        fileName = aFileName;
    }
    else
    {
#ifdef __linux__
        fileName = getenv("HOME");
        fileName += "/.KhunPhan.xml";
#else
#ifdef _WIN32
        PWSTR pwszPath;
        char pszPath[MAX_PATH] = "";

        if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData,
                                           KF_FLAG_CREATE, nullptr,
                                           &pwszPath)))
        {
            WideCharToMultiByte(CP_ACP, 0, pwszPath, -1, pszPath, MAX_PATH,
                                nullptr, nullptr);
            CoTaskMemFree(pwszPath);
            PathAppend(pszPath, "KhunPhan");
            PathAddBackslash(pszPath);

            if (!PathFileExists(pszPath))
            {
                SHCreateDirectoryEx(nullptr, pszPath, nullptr);
            }
        }

        fileName = pszPath;
        fileName += "KhunPhan.xml";
#else
#error "Unsupported platform"
#endif
#endif
    }
}

std::string KPConfig::GetDirectoryBase(tKPDir directoryID) const
{
    switch (directoryID)
    {
        case KP_TEXTURE_DIR:
            return "Textures" PATHSEPARATORSTRING;

        case KP_SOUND_DIR:
            return "Sounds" PATHSEPARATORSTRING;

        case KP_MUSIC_DIR:
            return "Music" PATHSEPARATORSTRING;

        case KP_LOCALE_DIR:
            return "locale" PATHSEPARATORSTRING;

        default:
            return "";
    }
}

std::string KPConfig::GetDirectory(tKPDir directoryID) const
{
#ifdef _WIN32
    return GetDirectoryBase(directoryID);
#else
#ifdef __linux__
    struct stat sStat;

    auto dir = GetDirectoryBase(directoryID);

    if (!dir.empty() && access(dir.c_str(), F_OK) == 0 &&
        stat(dir.c_str(), &sStat) == 0 && S_ISDIR(sStat.st_mode))
    {
        return dir;
    }

    dir = DATADIR "/" PACKAGE "/";
    dir += GetDirectoryBase(directoryID);

    if (access(dir.c_str(), F_OK) < 0 || stat(dir.c_str(), &sStat) < 0 ||
        !S_ISDIR(sStat.st_mode))
    {
        dir = "";
    }

    return dir;
#else
#error "Unsupported platform"
#endif
#endif
}

std::vector<std::string> KPConfig::GetTextureNames() const
{
    return BDirectory::GetSubDirectories(GetDirectory(KP_TEXTURE_DIR));
}


void KPConfig::WriteToFile() const
{
    xmlNsPtr ns = nullptr;
    auto doc  = xmlNewDoc(_TO("1.0"));
    doc->children  = xmlNewDocNode(doc, ns, _TO("KhunPhan"), nullptr);
    xmlSetProp(doc->children, _TO("Version"), _TO(VERSION));

    /******************** DisplaySettings *******************/
    auto tree = xmlNewChild(doc->children, ns, _TO("DisplaySettings"), nullptr);

    xmlNewTextChild(tree, ns, _TO("TextureName"), _TO(TextureName.c_str()));

    std::ostringstream iss1;
    iss1 << TextureSize;
    xmlNewChild(tree, ns, _TO("TextureSize"), _TO(iss1.str().c_str()));

    std::ostringstream iss2;
    iss2 << MenuTextureSize;
    xmlNewChild(tree, ns, _TO("MenuTextureSize"), _TO(iss2.str().c_str()));

    xmlNewChild(tree, ns, _TO("TextureType"), _TO(Nearest ? "1" : "0"));

    std::ostringstream iss3;
    iss3 << ScreenXResolution;
    xmlNewChild(tree, ns, _TO("ScreenXResolution"), _TO(iss3.str().c_str()));

    std::ostringstream iss4;
    iss4 << ColorDepth;
    xmlNewChild(tree, ns, _TO("ColorDepth"), _TO(iss4.str().c_str()));

    xmlNewChild(tree, ns, _TO("Reflections"), _TO(Reflections ? "1" : "0"));

    xmlNewChild(tree, ns, _TO("DisplayFPS"), _TO(DisplayFPS ? "1" : "0"));

    xmlNewChild(tree, ns, _TO("AmbientLight"), _TO(AmbientLight ? "1" : "0"));

    std::ostringstream iss5;
    iss5 << LightSources;
    xmlNewChild(tree, ns, _TO("LightSources"), _TO(iss5.str().c_str()));

    xmlNewChild(tree, ns, _TO("Shadows"), _TO(Shadows ? "1" : "0"));

    xmlNewChild(tree, ns, _TO("FullScreen"), _TO(FullScreen ? "1" : "0"));

    std::ostringstream iss6;
    iss6 << UserInterface;
    xmlNewChild(tree, ns, _TO("UserInterface"), _TO(iss6.str().c_str()));

    std::ostringstream iss7;
    iss7 << CameraPosition;
    xmlNewChild(tree, ns, _TO("CameraPosition"), _TO(iss7.str().c_str()));

    /******************** Game Control *******************/
    tree = xmlNewChild(doc->children, ns, _TO("GameControl"), nullptr);

    std::ostringstream iss8;
    iss8 << MouseSpeed;
    xmlNewChild(tree, ns, _TO("MouseSpeed"), _TO(iss8.str().c_str()));

    std::ostringstream iss9;
    iss9 << SolutionHint;
    xmlNewChild(tree, ns, _TO("SolutionHint"), _TO(iss9.str().c_str()));

    /******************** Audio/Music Settings  *******************/
    tree = xmlNewChild(doc->children, ns, _TO("AudioSettings"), nullptr);

    std::ostringstream iss10;
    iss10 << SoundVolume;
    xmlNewChild(tree, ns, _TO("SoundVolume"), _TO(iss10.str().c_str()));

    std::ostringstream iss11;
    iss11 << MusicVolume;
    xmlNewChild(tree, ns, _TO("MusicVolume"), _TO(iss11.str().c_str()));

    xmlNewChild(tree, ns, _TO("SoundOn"), _TO(SoundOn ? "1" : "0"));

    xmlNewChild(tree, ns, _TO("MusicOn"), _TO(MusicOn ? "1" : "0"));

    /******************** Language Settings  *******************/
    tree = xmlNewChild(doc->children, nullptr, _TO("LanguageSettings"),
                       nullptr);

    std::ostringstream iss12;
    iss12 << Language;
    xmlNewChild(tree, ns, _TO("Language"), _TO(iss12.str().c_str()));

    /******************** Debug Settings  *******************/
    tree = xmlNewChild(doc->children, ns, _TO("DebugSettings"), nullptr);

    if (PerformanceLog)
    {
        xmlNewChild(tree, ns, _TO("PerformanceLog"),
                    _TO(PerformanceLog ? "1" : "0"));
    }

    if (SkipProgressBar)
    {
        xmlNewChild(tree, ns, _TO("SkipProgressBar"),
                    _TO(SkipProgressBar ? "1" : "0"));
    }

    /******************** Saved Game *******************/
    tree = xmlNewChild(doc->children, ns, _TO("SavedGames"), nullptr);

    if (SavedGame != 0)
    {
        auto subtree = xmlNewChild(tree, ns, _TO("SavedGame"), nullptr);

        std::ostringstream iss1;
        iss1 << SavedGame;
        xmlNewChild(subtree, ns, _TO("Position"), _TO(iss1.str().c_str()));

        std::ostringstream iss2;
        iss2 << PlayTime;
        xmlNewChild(subtree, ns, _TO("PlayTime"), _TO(iss2.str().c_str()));

        std::ostringstream iss3;
        iss3 << Moves;
        xmlNewChild(subtree, ns, _TO("Moves"), _TO(iss3.str().c_str()));

        std::ostringstream iss4;
        iss4 << MovesWithHint;
        xmlNewChild(subtree, ns, _TO("MovesWithHint"), _TO(iss4.str().c_str()));

        std::ostringstream iss5;
        iss5 << CheatCount;
        xmlNewChild(subtree, ns, _TO("CheatCount"), _TO(iss5.str().c_str()));
    }

    xmlSaveFormatFile(GetFileName().c_str(), doc, 1);

    xmlFreeDoc(doc);
}

void KPConfig::ReadFromFile()
{
    auto doc = xmlParseFile(GetFileName().c_str());

    if (doc == nullptr)
    {
        return;
    }

    auto cur = doc->xmlChildrenNode;

    if (cur == nullptr || (cur->ns != nullptr) ||
        xmlStrcmp(cur->name, _TO("KhunPhan")))
    {
        xmlFreeDoc(doc);
        return;
    }

    auto version = xmlGetProp(cur, _TO("Version"));

    if (version != nullptr)
    {
        fileVersion = _FROM(version);
    }

    xmlFree(version);

    auto tree = cur->xmlChildrenNode;

    while (tree != nullptr)
    {
        if (!xmlStrcmp(tree->name, _TO("DisplaySettings")))
        {
            auto subtree = tree->xmlChildrenNode;

            while (subtree != nullptr)
            {
                auto tag = subtree->name;
                auto node = subtree->xmlChildrenNode;
                int tmp;

                if (!xmlStrcmp(tag, _TO("TextureName")))
                {
                    auto key = xmlNodeListGetString(doc, node, 1);
                    TextureName = _FROM(key);
                    xmlFree(key);
                }
                else if (!xmlStrcmp(tag, _TO("TextureSize")))
                {
                    auto key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> TextureSize;
                    xmlFree(key);
                }
                else if (!xmlStrcmp(tag, _TO("MenuTextureSize")))
                {
                    auto key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> MenuTextureSize;
                    xmlFree(key);
                }
                else if (!xmlStrcmp(tag, _TO("TextureType")))
                {
                    auto key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> tmp;
                    Nearest = (tmp != 0);
                    xmlFree(key);
                }
                else if (!xmlStrcmp(tag, _TO("ScreenXResolution")))
                {
                    auto key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> ScreenXResolution;
                    xmlFree(key);
                }
                else if (!xmlStrcmp(tag, _TO("DisplayFPS")))
                {
                    auto key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> tmp;
                    DisplayFPS = (tmp != 0);
                    xmlFree(key);
                }
                else if (!xmlStrcmp(tag, _TO("AmbientLight")))
                {
                    auto key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> tmp;
                    AmbientLight = (tmp != 0);
                    xmlFree(key);
                }
                else if (!xmlStrcmp(tag, _TO("LightSources")))
                {
                    auto key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> LightSources;
                    xmlFree(key);
                }
                else if (!xmlStrcmp(tag, _TO("FullScreen")))
                {
                    auto key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> tmp;
                    FullScreen = (tmp != 0);
                    xmlFree(key);
                }
                else if (!xmlStrcmp(tag, _TO("UserInterface")))
                {
                    auto key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> UserInterface;
                    xmlFree(key);
#if !defined(HAVE_LIBGLUT) && !defined(HAVE_LIBOPENGLUT)

                    if (UserInterface == 1)
                    {
                        UserInterface = 0;
                    }

#endif
                }
                else if (!xmlStrcmp(tag, _TO("CameraPosition")))
                {
                    auto key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> CameraPosition;
                    xmlFree(key);
                }

                subtree = subtree->next;
            }
        }
        else if (!xmlStrcmp(tree->name, _TO("GameControl")))
        {
            auto subtree = tree->xmlChildrenNode;

            while (subtree != nullptr)
            {
                auto tag = subtree->name;
                auto node = subtree->xmlChildrenNode;

                if (!xmlStrcmp(tag, _TO("MouseSpeed")))
                {
                    auto key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> MouseSpeed;
                    xmlFree(key);
                }
                else if (!xmlStrcmp(tag, _TO("SolutionHint")))
                {
                    auto key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> SolutionHint;
                    xmlFree(key);
                }

                subtree = subtree->next;
            }
        }
        else if (!xmlStrcmp(tree->name, _TO("AudioSettings")))
        {
            auto subtree = tree->xmlChildrenNode;

            while (subtree != nullptr)
            {
                auto tag = subtree->name;
                auto node = subtree->xmlChildrenNode;
                int tmp;

                if (!xmlStrcmp(tag, _TO("SoundVolume")))
                {
                    auto key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> SoundVolume;
                    xmlFree(key);
                }
                else if (!xmlStrcmp(tag, _TO("MusicVolume")))
                {
                    auto key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> MusicVolume;
                    xmlFree(key);
                }
                else if (!xmlStrcmp(tag, _TO("SoundOn")))
                {
                    auto key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> tmp;
                    SoundOn = (tmp != 0);
                    xmlFree(key);
                }
                else if (!xmlStrcmp(tag, _TO("MusicOn")))
                {
                    auto key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> tmp;
                    MusicOn = (tmp != 0);
                    xmlFree(key);
                }

                subtree = subtree->next;
            }
        }

        if (!xmlStrcmp(tree->name, _TO("LanguageSettings")))
        {
            auto subtree = tree->xmlChildrenNode;

            while (subtree != nullptr)
            {
                auto node = subtree->xmlChildrenNode;

                if (!xmlStrcmp(subtree->name, _TO("Language")))
                {
                    auto key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> Language;
                    xmlFree(key);
                }

                subtree = subtree->next;
            }
        }

        if (!xmlStrcmp(tree->name, _TO("DebugSettings")))
        {
            auto subtree = tree->xmlChildrenNode;

            while (subtree != nullptr)
            {
                auto node = subtree->xmlChildrenNode;
                int tmp;

                if (!xmlStrcmp(subtree->name, _TO("PerformanceLog")))
                {
                    auto key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> tmp;
                    PerformanceLog = (tmp != 0);
                    xmlFree(key);
                }
                else if (!xmlStrcmp(subtree->name, _TO("SkipProgressBar")))
                {
                    auto key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> tmp;
                    SkipProgressBar = (tmp != 0);
                    xmlFree(key);
                }

                subtree = subtree->next;
            }
        }
        else if (!xmlStrcmp(tree->name, _TO("SavedGames")))
        {
            auto subtree = tree->xmlChildrenNode;

            while (subtree != nullptr)
            {
                if (!xmlStrcmp(subtree->name, _TO("SavedGame")))
                {
                    auto subtree1 = subtree->xmlChildrenNode;

                    while (subtree1 != nullptr)
                    {
                        auto tag = subtree1->name;
                        auto node = subtree1->xmlChildrenNode;

                        if (!xmlStrcmp(tag, _TO("Position")))
                        {
                            auto key = xmlNodeListGetString(doc, node, 1);
                            std::istringstream iss(_FROM(key));
                            iss >> SavedGame;
                            xmlFree(key);
                        }
                        else if (!xmlStrcmp(tag, _TO("PlayTime")))
                        {
                            auto key = xmlNodeListGetString(doc, node, 1);
                            std::istringstream iss(_FROM(key));
                            iss >> PlayTime;
                            xmlFree(key);
                        }
                        else if (!xmlStrcmp(tag, _TO("Moves")))
                        {
                            auto key = xmlNodeListGetString(doc, node, 1);
                            std::istringstream iss(_FROM(key));
                            iss >> Moves;
                            xmlFree(key);
                        }
                        else if (!xmlStrcmp(tag, _TO("MovesWithHint")))
                        {
                            auto key = xmlNodeListGetString(doc, node, 1);
                            std::istringstream iss(_FROM(key));
                            iss >> MovesWithHint;
                            xmlFree(key);
                        }
                        else if (!xmlStrcmp(tag, _TO("CheatCount")))
                        {
                            auto key = xmlNodeListGetString(doc, node, 1);
                            std::istringstream iss(_FROM(key));
                            iss >> CheatCount;
                            xmlFree(key);
                        }

                        subtree1 = subtree1->next;
                    }
                }

                subtree = subtree->next;
            }
        }

        tree = tree->next;
    }

    xmlFreeDoc(doc);
}

void KPConfig::SetDefaultValues()
{
    //Default values
    FullScreen           = false;
    Nearest              = true;
    AmbientLight         = true;
    Reflections          = false;
    Shadows              = false;
    DisplayFPS           = false;
    SoundOn              = true;
    MusicOn              = true;

    ScreenXResolution    = 640;
    ColorDepth           = 32;
    TextureSize          = 4;
    MenuTextureSize      = 2;
    LightSources         = 3;
    Language             = 0;
    UserInterface        = 0;
    CameraPosition       = 2;

    SolutionHint         = 0;
    MouseSpeed           = 0.4f;

    SoundVolume          = 100;
    MusicVolume          = 100;

    SavedGame            = 0;
    PlayTime             = 0;
    Moves                = 0;
    MovesWithHint        = 0;
    CheatCount           = 0;

    TextureName          = "wood";

    PerformanceLog       = false;
    SkipProgressBar      = false;

    DisplayVersionOnly   = false;
}

void KPConfig::ReadCommandLineParams(int argc, char **argv)
{

    auto i = 1;

    while (i < argc)
    {
        if (!strcmp(argv[i], "-n") || !strcmp(argv[i], "-nearest"))
        {
            Nearest = true;
        }
        else if (!strcmp(argv[i], "-p") || !strcmp(argv[i], "-performancelog"))
        {
            PerformanceLog = true;
        }
        else if (!strcmp(argv[i], "-f") || !strcmp(argv[i], "-fullscreen"))
        {
            FullScreen = true;
        }
        else if (!strcmp(argv[i], "-w") || !strcmp(argv[i], "-windowed"))
        {
            FullScreen = false;
        }
        else if (!strcmp(argv[i], "-hq") || !strcmp(argv[i], "-highquality"))
        {
            TextureSize          = 2;
            MenuTextureSize      = 2;
            ScreenXResolution    = 1024;
        }
        else if (!strcmp(argv[i], "-t") || !strcmp(argv[i], "-textures"))
        {
            i++;

            if (!strcmp(argv[i], "1") || !strcmp(argv[i], "full"))
            {
                TextureSize = 1;
            }
            else if (!strcmp(argv[i], "2") || !strcmp(argv[i], "half"))
            {
                TextureSize = 2;
            }
            else if (!strcmp(argv[i], "4") || !strcmp(argv[i], "quater"))
            {
                TextureSize = 4;
            }
            else if (!strcmp(argv[i], "8") || !strcmp(argv[i], "eighth"))
            {
                TextureSize = 8;
            }
            else if (!strcmp(argv[i], "0") || !strcmp(argv[i], "off"))
            {
                TextureSize = 0;
            }
        }
        else if (!strcmp(argv[i], "-at") || !strcmp(argv[i], "-mtextures"))
        {
            i++;

            if (!strcmp(argv[i], "1") || !strcmp(argv[i], "voll"))
            {
                MenuTextureSize = 1;
            }
            else if (!strcmp(argv[i], "2") || !strcmp(argv[i], "half"))
            {
                MenuTextureSize = 2;
            }
            else if (!strcmp(argv[i], "4") || !strcmp(argv[i], "quater"))
            {
                MenuTextureSize = 4;
            }
            else if (!strcmp(argv[i], "8") || !strcmp(argv[i], "eighth"))
            {
                MenuTextureSize = 8;
            }
            else if (!strcmp(argv[i], "0") || !strcmp(argv[i], "off"))
            {
                MenuTextureSize = 0;
            }
        }
        else if (!strcmp(argv[i], "-m") || !strcmp(argv[i], "-mousespeed"))
        {
            i++;

            std::istringstream iss(argv[i]);

            iss >> MouseSpeed;
        }
        else if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--version"))
        {
            DisplayVersionOnly = true;
        }

        i++;
    }
}

std::string KPConfig::GetUserInterfaceName(int ui)
{
    switch (ui)
    {
        case 0:
            return "SDL";

        case 1:
            return "GLUT";

        default:
            return "<unknown>";
    }
}

void KPConfig::DebugPrint() const
{
    BLogger::Log("Current KhunPhan Configuration:");
    BLogger::Log("  UserInterface :        ",
                 GetUserInterfaceName(UserInterface));
    BLogger::Log("  TextureName :          ", TextureName);
    BLogger::Log("  TextureSize :          ", TextureSize);
    BLogger::Log("  MenuTextureSize :      ", MenuTextureSize);
    BLogger::Log("  TextureInterpolation : ", ON_OFF(Nearest));
    BLogger::Log("  ScreenXResolution :    ", ScreenXResolution);
    BLogger::Log("  ColorDepth :           ", ColorDepth);
    BLogger::Log("  DisplayFPS :           ", ON_OFF(DisplayFPS));
    BLogger::Log("  AmbientLight :         ", ON_OFF(AmbientLight));
    BLogger::Log("  LightSources :         ", LightSources);
    BLogger::Log("  FullScreen :           ", ON_OFF(FullScreen));
    BLogger::Log("  Language :             ", Language);
    BLogger::Log("  CameraPosition :       ", CameraPosition);
    BLogger::Log("  SolutionHint :         ", SolutionHint);
    BLogger::Log(std::fixed, std::setprecision(2),
                 "  MouseSpeed :           ", MouseSpeed);
    BLogger::Log("  SoundVolume :          ", SoundVolume);
    BLogger::Log("  MusicVolume :          ", MusicVolume);
    BLogger::Log("  SoundOn :              ", ON_OFF(SoundOn));
    BLogger::Log("  MusicOn :              ", ON_OFF(MusicOn));
    BLogger::Log("  SavedGame :            ", SavedGame);
    BLogger::Log("  PlayTime :             ", PlayTime, " ms");
    BLogger::Log("  Moves :                ", Moves);
    BLogger::Log("  MovesWithHint :        ", MovesWithHint);
    BLogger::Log("  CheatCount :           ", CheatCount);
    BLogger::Log("  PerformanceLog :       ", ON_OFF(PerformanceLog));
    BLogger::Log("  SkipProgressBar :      ", ON_OFF(SkipProgressBar));
}

