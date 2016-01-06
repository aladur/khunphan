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


#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#include "shlobj.h"
#endif
#include "misc1.h"
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

#define _TO(type)   reinterpret_cast<const xmlChar *>(type)
#define _FROM(type) reinterpret_cast<const char *>(type)

KPConfig *KPConfig::instance = NULL;

KPConfig::KPConfig() :
    FullScreen(false), Nearest(true), AmbientLight(false),
    Reflections(false), Shadows(false), DisplayFPS(false),
    SoundOn(true), MusicOn(true),
    ScreenXResolution(640), ColorDepth(16), TextureSize(2),
    MenuTextureSize(2), LightSources(2), Language(0),
    UserInterface(0), CameraPosition(2),
    SolutionHint(0), MouseSpeed(0.4f),
    SoundVolume(100), MusicVolume(100),
    SavedGame(0), PlayTime(0), Moves(0), MovesWithHint(0),
    CheatCount(0),
    TextureName("wood"),
    PerformanceLog(false), SkipProgressBar(false)
{
}

KPConfig::~KPConfig()
{
    xmlCleanupParser();
}

KPConfig &KPConfig::Instance()
{
    if (instance == NULL)
    {
        instance = new KPConfig;
        atexit (KPConfig::finalize);
    };
    return *instance;
}

std::string KPConfig::GetFileName()
{
    // Lazy initialization
    if (fileName.length() == 0)
    {
        SetFileName(NULL);
    }

    return fileName;
}

std::string KPConfig::GetFileVersion()
{
    return fileVersion;
}

// if aFileName is NULL initialize it to the default
void KPConfig::SetFileName(const char *aFileName)
{
    if (aFileName != NULL)
    {
        fileName = aFileName;
    }
    else
    {
#ifdef LINUX
        fileName = getenv("HOME");
        fileName += "/.KhunPhan.xml";
#endif
#ifdef WIN32
        PWSTR pwszPath;
        char pszPath[MAX_PATH] = "";

        if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData,
                                           KF_FLAG_CREATE, NULL,
                                           &pwszPath)))
        {
            WideCharToMultiByte(CP_ACP, 0, pwszPath, -1, pszPath, MAX_PATH,
                                NULL, NULL);
            CoTaskMemFree(pwszPath);
            PathAppend(pszPath, "KhunPhan");
            PathAddBackslash(pszPath);
            if (!PathFileExists(pszPath))
            {
                SHCreateDirectoryEx(NULL, pszPath, NULL);
            }
        }
        fileName = pszPath;
        fileName += "KhunPhan.xml";
#endif
    }
}

std::string KPConfig::GetDirectory(tKPDir directoryID) const
{
#ifdef WIN32
    switch (directoryID)
    {
        case KP_TEXTURE_DIR:
            return "Textures\\";
        case KP_SOUND_DIR:
            return "Sounds\\";
        case KP_MUSIC_DIR:
            return "Music\\";
        case KP_LOCALE_DIR:
            return "locale\\";
        default:
            return "";
    }
#endif
#ifdef LINUX
    std::string dir;
    struct stat sStat;

    switch (directoryID)
    {
        case KP_TEXTURE_DIR:
            dir = "Textures/";
            break;
        case KP_SOUND_DIR:
            dir = "Sounds/";
            break;
        case KP_MUSIC_DIR:
            dir = "Music/";
            break;
        case KP_LOCALE_DIR:
            dir = "locale/";
            break;
        default:
            dir = "";
    }
    if (!dir.empty() && access(dir.c_str(), F_OK) == 0 &&
        stat(dir.c_str(), &sStat) == 0 && S_ISDIR(sStat.st_mode))
    {
        return dir;
    }

    dir = DATADIR "/" PACKAGE "/";
    switch (directoryID)
    {
        case KP_TEXTURE_DIR:
            dir += "Textures/";
            break;
        case KP_SOUND_DIR:
            dir += "Sounds/";
            break;
        case KP_MUSIC_DIR:
            dir += "Music/";
            break;
        case KP_LOCALE_DIR:
            dir += "locale/";
            break;
    }
    if (access(dir.c_str(), F_OK) < 0 || stat(dir.c_str(), &sStat) < 0 ||
        !S_ISDIR(sStat.st_mode))
    {
        dir = "";
    }

    return dir;
#endif
}

std::vector<std::string> KPConfig::GetTextureNames() const
{
    return BDirectory::GetSubDirectories(GetDirectory(KP_TEXTURE_DIR));
}


void KPConfig::WriteToFile()
{
    xmlNsPtr ns = NULL;
    xmlDocPtr doc  = xmlNewDoc(_TO("1.0"));
    doc->children  = xmlNewDocNode(doc, ns, _TO("KhunPhan"), NULL);
    xmlSetProp(doc->children, _TO("Version"), _TO(VERSION));

    /******************** DisplaySettings *******************/
    xmlNodePtr tree = xmlNewChild(doc->children, ns, _TO("DisplaySettings"),
                                  NULL);

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
    tree = xmlNewChild(doc->children, ns, _TO("GameControl"), NULL);

    std::ostringstream iss8;
    iss8 << MouseSpeed;
    xmlNewChild(tree, ns, _TO("MouseSpeed"), _TO(iss8.str().c_str()));

    std::ostringstream iss9;
    iss9 << SolutionHint;
    xmlNewChild(tree, ns, _TO("SolutionHint"), _TO(iss9.str().c_str()));

    /******************** Audio/Music Settings  *******************/
    tree = xmlNewChild(doc->children, ns, _TO("AudioSettings"), NULL);

    std::ostringstream iss10;
    iss10 << SoundVolume;
    xmlNewChild(tree, ns, _TO("SoundVolume"), _TO(iss10.str().c_str()));

    std::ostringstream iss11;
    iss11 << MusicVolume;
    xmlNewChild(tree, ns, _TO("MusicVolume"), _TO(iss11.str().c_str()));

    xmlNewChild(tree, ns, _TO("SoundOn"), _TO(SoundOn ? "1" : "0"));

    xmlNewChild(tree, ns, _TO("MusicOn"), _TO(MusicOn ? "1" : "0"));

    /******************** Language Settings  *******************/
    tree = xmlNewChild(doc->children, NULL, _TO("LanguageSettings"), NULL);

    std::ostringstream iss12;
    iss12 << Language;
    xmlNewChild(tree, ns, _TO("Language"), _TO(iss12.str().c_str()));

    /******************** Debug Settings  *******************/
    tree = xmlNewChild(doc->children, ns, _TO("DebugSettings"), NULL);

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
    tree = xmlNewChild(doc->children, ns, _TO("SavedGames"), NULL);

    if (SavedGame != 0)
    {
        xmlNodePtr subtree = xmlNewChild(tree, ns, _TO("SavedGame"), NULL);

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
    xmlDocPtr doc = xmlParseFile(GetFileName().c_str());

    if (doc == NULL)
    {
        return;
    }

    xmlNodePtr cur = doc->xmlChildrenNode;

    if (cur == NULL || (cur->ns != NULL) ||
        xmlStrcmp(cur->name, _TO("KhunPhan")))
    {
        xmlFreeDoc(doc);
        return;
    }

    xmlChar *version = xmlGetProp(cur, _TO("Version"));
    if (version != NULL)
    {
        fileVersion = _FROM(version);
    }

    xmlNodePtr tree = cur->xmlChildrenNode;

    while (tree != NULL)
    {
        if (!xmlStrcmp(tree->name, _TO("DisplaySettings")))
        {
            xmlNodePtr subtree = tree->xmlChildrenNode;

            while (subtree != NULL)
            {
                const xmlChar *tag = subtree->name;
                xmlNodePtr node    = subtree->xmlChildrenNode;
                xmlChar *key;
                int tmp;

                if (!xmlStrcmp(tag, _TO("TextureName")))
                {
                    key = xmlNodeListGetString(doc, node, 1);
                    TextureName = _FROM(key);
                    xmlFree(key);
                }
                else if (!xmlStrcmp(tag, _TO("TextureSize")))
                {
                    key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> TextureSize;
                    xmlFree(key);
                }
                else if (!xmlStrcmp(tag, _TO("MenuTextureSize")))
                {
                    key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> MenuTextureSize;
                    xmlFree(key);
                }
                else if (!xmlStrcmp(tag, _TO("TextureType")))
                {
                    key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> tmp;
                    Nearest = (tmp != 0);
                    xmlFree(key);
                }
                else if (!xmlStrcmp(tag, _TO("ScreenXResolution")))
                {
                    key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> ScreenXResolution;
                    xmlFree(key);
                }
                else if (!xmlStrcmp(tag, _TO("ColorDepth")))
                {
                    key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> ColorDepth;
                    xmlFree(key);
                }
                else if (!xmlStrcmp(tag, _TO("Reflections")))
                {
                    key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> tmp;
                    Reflections = (tmp != 0);
                    xmlFree(key);
                }
                else if (!xmlStrcmp(tag, _TO("DisplayFPS")))
                {
                    key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> tmp;
                    DisplayFPS = (tmp != 0);
                    xmlFree(key);
                }
                else if (!xmlStrcmp(tag, _TO("AmbientLight")))
                {
                    key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> tmp;
                    AmbientLight = (tmp != 0);
                    xmlFree(key);
                }
                else if (!xmlStrcmp(tag, _TO("LightSources")))
                {
                    key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> LightSources;
                    xmlFree(key);
                }
                else if (!xmlStrcmp(tag, _TO("Shadows")))
                {
                    key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> tmp;
                    Shadows = (tmp != 0);
                    xmlFree(key);
                }
                else if (!xmlStrcmp(tag, _TO("FullScreen")))
                {
                    key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> tmp;
                    FullScreen = (tmp != 0);
                    xmlFree(key);
                }
                else if (!xmlStrcmp(tag, _TO("UserInterface")))
                {
                    key = xmlNodeListGetString(doc, node, 1);
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
                    key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> CameraPosition;
                    xmlFree(key);
                }

                subtree = subtree->next;
            }
        }
        else if (!xmlStrcmp(tree->name, _TO("GameControl")))
        {
            xmlNodePtr subtree = tree->xmlChildrenNode;

            while (subtree != NULL)
            {
                const xmlChar *tag = subtree->name;
                xmlNodePtr node    = subtree->xmlChildrenNode;
                xmlChar *key;

                if (!xmlStrcmp(tag, _TO("MouseSpeed")))
                {
                    key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> MouseSpeed;
                    xmlFree(key);
                }
                else if (!xmlStrcmp(tag, _TO("SolutionHint")))
                {
                    key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> SolutionHint;
                    xmlFree(key);
                }

                subtree = subtree->next;
            }
        }
        else if (!xmlStrcmp(tree->name, _TO("AudioSettings")))
        {
            xmlNodePtr subtree = tree->xmlChildrenNode;

            while (subtree != NULL)
            {
                const xmlChar *tag = subtree->name;
                xmlNodePtr node    = subtree->xmlChildrenNode;
                xmlChar *key;
                int tmp;

                if (!xmlStrcmp(tag, _TO("SoundVolume")))
                {
                    key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> SoundVolume;
                    xmlFree(key);
                }
                else if (!xmlStrcmp(tag, _TO("MusicVolume")))
                {
                    key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> MusicVolume;
                    xmlFree(key);
                }
                else if (!xmlStrcmp(tag, _TO("SoundOn")))
                {
                    key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> tmp;
                    SoundOn = (tmp != 0);
                    xmlFree(key);
                }
                else if (!xmlStrcmp(tag, _TO("MusicOn")))
                {
                    key = xmlNodeListGetString(doc, node, 1);
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
            xmlNodePtr subtree = tree->xmlChildrenNode;

            while (subtree != NULL)
            {
                xmlNodePtr node = subtree->xmlChildrenNode;

                if (!xmlStrcmp(subtree->name, _TO("Language")))
                {
                    xmlChar *key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> Language;
                    xmlFree(key);
                }
                subtree = subtree->next;
            }
        }
        if (!xmlStrcmp(tree->name, _TO("DebugSettings")))
        {
            xmlNodePtr subtree = tree->xmlChildrenNode;

            while (subtree != NULL)
            {
                xmlNodePtr node = subtree->xmlChildrenNode;
                int tmp;

                if (!xmlStrcmp(subtree->name, _TO("PerformanceLog")))
                {
                    xmlChar *key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> tmp;
                    PerformanceLog = (tmp != 0);
                    xmlFree(key);
                }
                else if (!xmlStrcmp(subtree->name, _TO("SkipProgressBar")))
                {
                    xmlChar *key = xmlNodeListGetString(doc, node, 1);
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
            xmlNodePtr subtree = tree->xmlChildrenNode;

            while (subtree != NULL)
            {
                if (!xmlStrcmp(subtree->name, _TO("SavedGame")))
                {
                    xmlNodePtr subtree1 = subtree->xmlChildrenNode;

                    while (subtree1 != NULL)
                    {
                        const xmlChar *tag = subtree1->name;
                        xmlNodePtr node    = subtree1->xmlChildrenNode;
                        xmlChar *key;

                        if (!xmlStrcmp(tag, _TO("Position")))
                        {
                            key = xmlNodeListGetString(doc, node, 1);
                            std::istringstream iss(_FROM(key));
                            iss >> SavedGame;
                            xmlFree(key);
                        }
                        else if (!xmlStrcmp(tag, _TO("PlayTime")))
                        {
                            key = xmlNodeListGetString(doc, node, 1);
                            std::istringstream iss(_FROM(key));
                            iss >> PlayTime;
                            xmlFree(key);
                        }
                        else if (!xmlStrcmp(tag, _TO("Moves")))
                        {
                            key = xmlNodeListGetString(doc, node, 1);
                            std::istringstream iss(_FROM(key));
                            iss >> Moves;
                            xmlFree(key);
                        }
                        else if (!xmlStrcmp(tag, _TO("MovesWithHint")))
                        {
                            key = xmlNodeListGetString(doc, node, 1);
                            std::istringstream iss(_FROM(key));
                            iss >> MovesWithHint;
                            xmlFree(key);
                        }
                        else if (!xmlStrcmp(tag, _TO("CheatCount")))
                        {
                            key = xmlNodeListGetString(doc, node, 1);
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
    ColorDepth           = 16;
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
}

void KPConfig::ReadCommandLineParams(int argc,char **argv)
{

    int i=1;
    while (i<argc)
    {
        if (!strcmp(argv[i],"-n")||!strcmp(argv[i],"-nearest"))
        {
            Nearest = true;
        }
        else if (!strcmp(argv[i],"-p")||!strcmp(argv[i],"-performancelog"))
        {
            PerformanceLog = true;
        }
        else if (!strcmp(argv[i],"-f")||!strcmp(argv[i],"-fullscreen"))
        {
            FullScreen = true;
        }
        else if (!strcmp(argv[i],"-w")||!strcmp(argv[i],"-windowed"))
        {
            FullScreen = false;
        }
        else if (!strcmp(argv[i],"-hq")||!strcmp(argv[i],"-highquality"))
        {
            TextureSize          = 2;
            MenuTextureSize      = 2;
            ScreenXResolution    = 1024;
            ColorDepth           = 32;
            Reflections          = true;
        }
        else if (!strcmp(argv[i],"-t")||!strcmp(argv[i],"-textures"))
        {
            i++;
            if (!strcmp(argv[i],"1")||!strcmp(argv[i],"full"))
            {
                TextureSize=1;
            }
            else if (!strcmp(argv[i],"2")||!strcmp(argv[i],"half"))
            {
                TextureSize=2;
            }
            else if (!strcmp(argv[i],"4")||!strcmp(argv[i],"quater"))
            {
                TextureSize=4;
            }
            else if (!strcmp(argv[i],"8")||!strcmp(argv[i],"eighth"))
            {
                TextureSize=8;
            }
            else if (!strcmp(argv[i],"0")||!strcmp(argv[i],"off"))
            {
                TextureSize=0;
            }
        }
        else if (!strcmp(argv[i],"-at")||!strcmp(argv[i],"-mtextures"))
        {
            i++;
            if (!strcmp(argv[i],"1")||!strcmp(argv[i],"voll"))
            {
                MenuTextureSize=1;
            }
            else if (!strcmp(argv[i],"2")||!strcmp(argv[i],"half"))
            {
                MenuTextureSize=2;
            }
            else if (!strcmp(argv[i],"4")||!strcmp(argv[i],"quater"))
            {
                MenuTextureSize=4;
            }
            else if (!strcmp(argv[i],"8")||!strcmp(argv[i],"eighth"))
            {
                MenuTextureSize=8;
            }
            else if (!strcmp(argv[i],"0")||!strcmp(argv[i],"off"))
            {
                MenuTextureSize=0;
            }
        }
        else if (!strcmp(argv[i],"-s")||!strcmp(argv[i],"-shadows"))
        {
            Shadows=true;
        }
        else if (!strcmp(argv[i],"-m")||!strcmp(argv[i],"-mousespeed"))
        {
            i++;

            std::istringstream iss(argv[i]);

            iss >> MouseSpeed;
        }

        i++;
    }
}

const char *KPConfig::GetUserInterfaceName(int ui)
{
    switch(ui)
    {
        case 0:
            return "SDL";
        case 1:
            return "GLUT";
        default:
            return "";
    }
}

void KPConfig::DebugPrint()
{
    LOG1("Current KhunPhan Configuration:");
    LOG2("  UserInterface :        ",
                GetUserInterfaceName(UserInterface));
    LOG2("  TextureName :          ", TextureName);
    LOG2("  TextureSize :          ", TextureSize);
    LOG2("  MenuTextureSize :      ", MenuTextureSize);
    LOG2("  Shadows :              ", ON_OFF(Shadows));
    LOG2("  TextureInterpolation : ", ON_OFF(Nearest));
    LOG2("  ScreenXResolution :    ", ScreenXResolution);
    LOG2("  ColorDepth :           ", ColorDepth);
    LOG2("  Reflections :          ", ON_OFF(Reflections));
    LOG2("  DisplayFPS :           ", ON_OFF(DisplayFPS));
    LOG2("  AmbientLight :         ", ON_OFF(AmbientLight));
    LOG2("  LightSources :         ", LightSources);
    LOG2("  FullScreen :           ", ON_OFF(FullScreen));
    LOG2("  Language :             ", Language);
    LOG2("  CameraPosition :       ", CameraPosition);
    LOG2("  SolutionHint :         ", SolutionHint);
    LOG4(std::fixed, std::setprecision(2),
         "  MouseSpeed :           ", MouseSpeed);
    LOG2("  SoundVolume :          ", SoundVolume);
    LOG2("  MusicVolume :          ", MusicVolume);
    LOG2("  SoundOn :              ", ON_OFF(SoundOn));
    LOG2("  MusicOn :              ", ON_OFF(MusicOn));
    LOG2("  SavedGame :            ", SavedGame);
    LOG3("  PlayTime :             ", PlayTime, " ms");
    LOG2("  Moves :                ", Moves);
    LOG2("  MovesWithHint :        ", MovesWithHint);
    LOG2("  CheatCount :           ", CheatCount);
    LOG2("  PerformanceLog :       ", ON_OFF(PerformanceLog));
    LOG2("  SkipProgressBar :      ", ON_OFF(SkipProgressBar));
}

