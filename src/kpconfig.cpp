/*
    kpconfig.cpp


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
#include <stdio.h>
#include <libxml/tree.h>
#include "kpconfig.h"
#include "bdir.h"


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
    xmlDocPtr doc;
    xmlNodePtr tree, subtree;
    xmlChar buffer[128];

    doc = xmlNewDoc((const xmlChar *)"1.0");
    doc->children = xmlNewDocNode(doc, NULL, (const xmlChar *)"KhunPhan",
                                  NULL);
    xmlSetProp(doc->children, (const xmlChar *)"Version",
               (const xmlChar *)VERSION);

    // Display Settings
    tree = xmlNewChild(doc->children, NULL, (xmlChar *)"DisplaySettings", NULL);
    sprintf((char *)buffer, "%s", TextureName.c_str());
    xmlNewChild(tree, NULL, (xmlChar *)"TextureName", buffer);
    sprintf((char *)buffer, "%d", TextureSize);
    xmlNewChild(tree, NULL, (xmlChar *)"TextureSize", buffer);
    sprintf((char *)buffer, "%d", MenuTextureSize);
    xmlNewChild(tree, NULL, (xmlChar *)"MenuTextureSize", buffer);
    sprintf((char *)buffer, "%d", Nearest ? 1 : 0);
    xmlNewChild(tree, NULL, (xmlChar *)"TextureType", buffer);
    sprintf((char *)buffer, "%d", ScreenXResolution);
    xmlNewChild(tree, NULL, (xmlChar *)"ScreenXResolution",buffer);
    sprintf((char *)buffer, "%d", ColorDepth);
    xmlNewChild(tree, NULL, (xmlChar *)"ColorDepth", buffer);
    sprintf((char *)buffer, "%d", Reflections ? 1 : 0);
    xmlNewChild(tree, NULL, (xmlChar *)"Reflections", buffer);
    sprintf((char *)buffer, "%d", DisplayFPS ? 1 : 0);
    xmlNewChild(tree, NULL, (xmlChar *)"DisplayFPS", buffer);
    sprintf((char *)buffer, "%d", AmbientLight ? 1 : 0);
    xmlNewChild(tree, NULL, (xmlChar *)"AmbientLight", buffer);
    sprintf((char *)buffer, "%d", LightSources);
    xmlNewChild(tree, NULL, (xmlChar *)"LightSources", buffer);
    sprintf((char *)buffer, "%d", Shadows ? 1 : 0);
    xmlNewChild(tree, NULL, (xmlChar *)"Shadows", buffer);
    sprintf((char *)buffer, "%d", FullScreen ? 1 : 0);
    xmlNewChild(tree, NULL, (xmlChar *)"FullScreen", buffer);
    sprintf((char *)buffer, "%d", UserInterface);
    xmlNewChild(tree, NULL, (xmlChar *)"UserInterface", buffer);
    sprintf((char *)buffer, "%d", CameraPosition);
    xmlNewChild(tree, NULL, (xmlChar *)"CameraPosition", buffer);

    // Game Control
    tree = xmlNewChild(doc->children, NULL, (xmlChar *)"GameControl", NULL);
    sprintf((char *)buffer, "%f", MouseSpeed);
    xmlNewChild(tree, NULL, (xmlChar *)"MouseSpeed", buffer);
    sprintf((char *)buffer, "%d", SolutionHint);
    xmlNewChild(tree, NULL, (xmlChar *)"SolutionHint", buffer);

    // Audio/Music Settings
    tree = xmlNewChild(doc->children, NULL, (xmlChar *)"AudioSettings", NULL);
    sprintf((char *)buffer, "%d", SoundVolume);
    xmlNewChild(tree, NULL, (xmlChar *)"SoundVolume", buffer);
    sprintf((char *)buffer, "%d", MusicVolume);
    xmlNewChild(tree, NULL, (xmlChar *)"MusicVolume", buffer);
    sprintf((char *)buffer, "%d", SoundOn ? 1 : 0);
    xmlNewChild(tree, NULL, (xmlChar *)"SoundOn", buffer);
    sprintf((char *)buffer, "%d", MusicOn ? 1 : 0);
    xmlNewChild(tree, NULL, (xmlChar *)"MusicOn", buffer);

    // Language Settings
    tree = xmlNewChild(doc->children, NULL, (xmlChar *)"LanguageSettings",
                        NULL);
    sprintf((char *)buffer, "%d", Language);
    xmlNewChild(tree, NULL, (xmlChar *)"Language", buffer);

    // Debug Settings
    tree = xmlNewChild(doc->children, NULL, (xmlChar *)"DebugSettings", NULL);
    if (PerformanceLog)
    {
        sprintf((char *)buffer, "%d", PerformanceLog ? 1 : 0);
        xmlNewChild(tree, NULL, (xmlChar *)"PerformanceLog", buffer);
    }
    if (SkipProgressBar)
    {
        sprintf((char *)buffer, "%d", SkipProgressBar ? 1 : 0);
        xmlNewChild(tree, NULL, (xmlChar *)"SkipProgressBar", buffer);
    }

    // Saved Game
    tree = xmlNewChild(doc->children, NULL, (xmlChar *)"SavedGames", NULL);
    if (SavedGame != 0)
    {
        subtree  = xmlNewChild(tree, NULL, (xmlChar *)"SavedGame", NULL);

        sprintf((char *)buffer, FMT_UINT64x, SavedGame);
        xmlNewChild(subtree, NULL, (xmlChar *)"Position", buffer);
        sprintf((char *)buffer, "%u", PlayTime);
        xmlNewChild(subtree, NULL, (xmlChar *)"PlayTime", buffer);
        sprintf((char *)buffer, "%u", Moves);
        xmlNewChild(subtree, NULL, (xmlChar *)"Moves", buffer);
        sprintf((char *)buffer, "%u", MovesWithHint);
        xmlNewChild(subtree, NULL, (xmlChar *)"MovesWithHint", buffer);
        sprintf((char *)buffer, "%u", CheatCount);
        xmlNewChild(subtree, NULL, (xmlChar *)"CheatCount", buffer);
    }

    xmlSaveFile (GetFileName().c_str(), doc);

    xmlFreeDoc(doc);
}

void KPConfig::ReadFromFile()
{
    xmlDocPtr  doc;
    xmlNodePtr cur, tree, subtree, subtree1;
    xmlNsPtr   ns = NULL;

    doc = xmlParseFile(GetFileName().c_str());
    if (doc == NULL)
    {
        return;
    }

    cur = doc->xmlChildrenNode;
    if (cur)
    {
        tree = cur->xmlChildrenNode;
        while (tree != NULL)
        {
            xmlChar   *p;

            if ((!xmlStrcmp(tree->name, (const xmlChar *)"DisplaySettings")) &&
                (cur->ns == ns))
            {
                subtree = tree->xmlChildrenNode;
                while (subtree != NULL)
                {
                    if ((!xmlStrcmp(subtree->name,
                                    (const xmlChar *)"TextureName")) &&
                        (cur->ns == ns) &&
                        (p = xmlNodeListGetString(doc,
                                                  subtree->xmlChildrenNode, 1)))
                    {
                        TextureName = (char *)p;
                        xmlFree(p);
                    }
                    if ((!xmlStrcmp(subtree->name,
                                    (const xmlChar *)"TextureSize")) &&
                        (cur->ns == ns) &&
                        (p = xmlNodeListGetString(doc,
                                                  subtree->xmlChildrenNode, 1)))
                    {
                        TextureSize = atoi((char *)p);
                        xmlFree(p);
                    }
                    if ((!xmlStrcmp(subtree->name,
                                    (const xmlChar *)"MenuTextureSize")) &&
                        (cur->ns == ns) &&
                        (p = xmlNodeListGetString(doc,
                                                  subtree->xmlChildrenNode, 1)))
                    {
                        MenuTextureSize = atoi((char *)p);
                        xmlFree(p);
                    }
                    if ((!xmlStrcmp(subtree->name,
                                    (const xmlChar *)"TextureType")) &&
                        (cur->ns == ns) &&
                        (p = xmlNodeListGetString(doc,
                                                  subtree->xmlChildrenNode, 1)))
                    {
                        Nearest = (atoi((char *)p) != 0);
                        xmlFree(p);
                    }
                    if ((!xmlStrcmp(subtree->name,
                                    (const xmlChar *)"ScreenXResolution")) &&
                        (cur->ns == ns) &&
                        (p = xmlNodeListGetString(doc,
                                                  subtree->xmlChildrenNode, 1)))
                    {
                        ScreenXResolution = atoi((char *)p);
                        xmlFree(p);
                    }
                    if ((!xmlStrcmp(subtree->name,
                                    (const xmlChar *)"ColorDepth")) &&
                        (cur->ns == ns) &&
                        (p = xmlNodeListGetString(doc,
                                                  subtree->xmlChildrenNode, 1)))
                    {
                        ColorDepth = atoi((char *)p);
                        xmlFree(p);
                    }
                    if ((!xmlStrcmp(subtree->name,
                                    (const xmlChar *)"Reflections")) &&
                        (cur->ns == ns) &&
                        (p = xmlNodeListGetString(doc,
                                                  subtree->xmlChildrenNode, 1)))
                    {
                        Reflections = (atoi((char *)p) != 0);
                        xmlFree(p);
                    }
                    if ((!xmlStrcmp(subtree->name,
                                    (const xmlChar *)"DisplayFPS")) &&
                        (cur->ns == ns) &&
                        (p = xmlNodeListGetString(doc,
                                                  subtree->xmlChildrenNode, 1)))
                    {
                        DisplayFPS = (atoi((char *)p) != 0);
                        xmlFree(p);
                    }
                    if ((!xmlStrcmp(subtree->name,
                                    (const xmlChar *)"AmbientLight")) &&
                        (cur->ns == ns) &&
                        (p = xmlNodeListGetString(doc,
                                                  subtree->xmlChildrenNode, 1)))
                    {
                        AmbientLight = (atoi((char *)p) != 0);
                        xmlFree(p);
                    }
                    if ((!xmlStrcmp(subtree->name,
                                    (const xmlChar *)"LightSources")) &&
                        (cur->ns == ns) &&
                        (p = xmlNodeListGetString(doc,
                                                  subtree->xmlChildrenNode, 1)))
                    {
                        LightSources = atoi((char *)p);
                        xmlFree(p);
                    }
                    if ((!xmlStrcmp(subtree->name,
                                    (const xmlChar *)"Shadows")) &&
                        (cur->ns == ns) &&
                        (p = xmlNodeListGetString(doc,
                                                  subtree->xmlChildrenNode, 1)))
                    {
                        Shadows = (atoi((char *)p) != 0);
                        xmlFree(p);
                    }
                    if ((!xmlStrcmp(subtree->name,
                                    (const xmlChar *)"FullScreen")) &&
                        (cur->ns == ns) &&
                        (p = xmlNodeListGetString(doc,
                                                  subtree->xmlChildrenNode, 1)))
                    {
                        FullScreen = (atoi((char *)p) != 0);
                        xmlFree(p);
                    }
                    if ((!xmlStrcmp(subtree->name,
                                    (const xmlChar *)"UserInterface")) &&
                        (cur->ns == ns) &&
                        (p = xmlNodeListGetString(doc,
                                                  subtree->xmlChildrenNode, 1)))
                    {
                        UserInterface = atoi((char *)p);
                        xmlFree(p);
                    }
#if !defined(HAVE_LIBGLUT) && !defined(HAVE_LIBOPENGLUT)
                    if (UserInterface == 1)
                    {
                        UserInterface = 0;
                    }
#endif
                    if ((!xmlStrcmp(subtree->name,
                                    (const xmlChar *)"CameraPosition")) &&
                        (cur->ns == ns) &&
                        (p = xmlNodeListGetString(doc,
                                                  subtree->xmlChildrenNode, 1)))
                    {
                        CameraPosition = atoi((char *)p);
                        xmlFree(p);
                    }
                    subtree = subtree->next;
                }
            }
            if ((!xmlStrcmp(tree->name, (const xmlChar *)"GameControl")) &&
                (cur->ns == ns))
            {
                subtree = tree->xmlChildrenNode;
                while (subtree != NULL)
                {
                    if ((!xmlStrcmp(subtree->name,
                                    (const xmlChar *)"MouseSpeed")) &&
                        (cur->ns == ns) &&
                        (p = xmlNodeListGetString(doc,
                                                  subtree->xmlChildrenNode, 1)))
                    {
                        MouseSpeed = (float)atof((char *)p);
                        xmlFree(p);
                    }
                    if ((!xmlStrcmp(subtree->name,
                                    (const xmlChar *)"SolutionHint")) &&
                        (cur->ns == ns) &&
                        (p = xmlNodeListGetString(doc,
                                                  subtree->xmlChildrenNode, 1)))
                    {
                        SolutionHint = atoi((char *)p);
                        xmlFree(p);
                    }
                    subtree = subtree->next;
                }
            }
            if ((!xmlStrcmp(tree->name, (const xmlChar *)"AudioSettings")) &&
                (cur->ns == ns))
            {
                subtree = tree->xmlChildrenNode;
                while (subtree != NULL)
                {
                    if ((!xmlStrcmp(subtree->name,
                                    (const xmlChar *)"SoundVolume")) &&
                        (cur->ns == ns) &&
                        (p = xmlNodeListGetString(doc,
                                                  subtree->xmlChildrenNode, 1)))
                    {
                        SoundVolume = atoi((char *)p);
                        xmlFree(p);
                    }
                    if ((!xmlStrcmp(subtree->name,
                                    (const xmlChar *)"MusicVolume")) &&
                        (cur->ns == ns) &&
                        (p = xmlNodeListGetString(doc,
                                                  subtree->xmlChildrenNode, 1)))
                    {
                        MusicVolume = atoi((char *)p);
                        xmlFree(p);
                    }
                    if ((!xmlStrcmp(subtree->name,
                                    (const xmlChar *)"SoundOn")) &&
                        (cur->ns == ns) &&
                        (p = xmlNodeListGetString(doc,
                                                  subtree->xmlChildrenNode, 1)))
                    {
                        SoundOn = (atoi((char *)p) != 0);
                        xmlFree(p);
                    }
                    if ((!xmlStrcmp(subtree->name,
                                    (const xmlChar *)"MusicOn")) &&
                        (cur->ns == ns) &&
                        (p = xmlNodeListGetString(doc,
                                                  subtree->xmlChildrenNode, 1)))
                    {
                        MusicOn = (atoi((char *)p) != 0);
                        xmlFree(p);
                    }
                    subtree = subtree->next;
                }
            }
            if ((!xmlStrcmp(tree->name, (const xmlChar *)"LanguageSettings")) &&
                (cur->ns == ns))
            {
                subtree = tree->xmlChildrenNode;
                while (subtree != NULL)
                {
                    if ((!xmlStrcmp(subtree->name,
                                    (const xmlChar *)"Language")) &&
                        (cur->ns == ns) &&
                        (p = xmlNodeListGetString(doc,
                                                  subtree->xmlChildrenNode, 1)))
                    {
                        Language = atoi((char *)p);
                        xmlFree(p);
                    }
                    subtree = subtree->next;
                }
            }
            if ((!xmlStrcmp(tree->name, (const xmlChar *)"DebugSettings")) &&
                (cur->ns == ns))
            {
                subtree = tree->xmlChildrenNode;
                while (subtree != NULL)
                {
                    if ((!xmlStrcmp(subtree->name,
                                    (const xmlChar *)"PerformanceLog")) &&
                        (cur->ns == ns) &&
                        (p = xmlNodeListGetString(doc,
                                                  subtree->xmlChildrenNode, 1)))
                    {
                        PerformanceLog = (atoi((char *)p) != 0);
                        xmlFree(p);
                    }
                    if ((!xmlStrcmp(subtree->name,
                                    (const xmlChar *)"SkipProgressBar")) &&
                        (cur->ns == ns) &&
                        (p = xmlNodeListGetString(doc,
                                                  subtree->xmlChildrenNode, 1)))
                    {
                        SkipProgressBar = (atoi((char *)p) != 0);
                        xmlFree(p);
                    }
                    subtree = subtree->next;
                }
            }
            if ((!xmlStrcmp(tree->name,
                            (const xmlChar *)"SavedGames")) && (cur->ns == ns))
            {
                subtree = tree->xmlChildrenNode;
                while (subtree != NULL)
                {
                    if ((!xmlStrcmp(subtree->name,
                                    (const xmlChar *)"SavedGame")) &&
                        (cur->ns == ns))
                    {
                        subtree1 = subtree->xmlChildrenNode;
                        while (subtree1 != NULL)
                        {
                            if ((!xmlStrcmp(subtree1->name,
                                            (const xmlChar *)"Position")) &&
                                (cur->ns == ns) &&
                                (p = xmlNodeListGetString(doc,
                                                 subtree1->xmlChildrenNode, 1)))
                            {
                                sscanf((const char *)p, FMT_UINT64x,
                                       &SavedGame);
                                xmlFree(p);
                            }
                            if ((!xmlStrcmp(subtree1->name,
                                            (const xmlChar *)"PlayTime")) &&
                                (cur->ns == ns) &&
                                (p = xmlNodeListGetString(doc,
                                                 subtree1->xmlChildrenNode, 1)))
                            {
                                PlayTime = atoi((char *)p);
                                xmlFree(p);
                            }
                            if ((!xmlStrcmp(subtree1->name,
                                            (const xmlChar *)"Moves")) &&
                                (cur->ns == ns) &&
                                (p = xmlNodeListGetString(doc,
                                                 subtree1->xmlChildrenNode, 1)))
                            {
                                Moves = atoi((char *)p);
                                xmlFree(p);
                            }
                            if ((!xmlStrcmp(subtree1->name,
                                           (const xmlChar *)"MovesWithHint")) &&
                                (cur->ns == ns) &&
                                (p = xmlNodeListGetString(doc,
                                                 subtree1->xmlChildrenNode, 1)))
                            {
                                MovesWithHint = atoi((char *)p);
                                xmlFree(p);
                            }
                            if ((!xmlStrcmp(subtree1->name,
                                              (const xmlChar *)"CheatCount")) &&
                                (cur->ns == ns) &&
                                (p = xmlNodeListGetString(doc,
                                                 subtree1->xmlChildrenNode, 1)))
                            {
                                CheatCount = atoi((char *)p);
                                xmlFree(p);
                            }
                            subtree1 = subtree1->next;
                        }
                    }
                    subtree = subtree->next;
                }
            }
            tree = tree->next;
        }
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
            MouseSpeed = (float)atof(argv[i]);
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
    DEBUGPRINT("Current KhunPhan Configuration:\n");
    DEBUGPRINT1("  UserInterface :        %s\n",
                GetUserInterfaceName(UserInterface));
    DEBUGPRINT1("  TextureName :          %s\n", TextureName.c_str());
    DEBUGPRINT1("  TextureSize :          %d\n", TextureSize);
    DEBUGPRINT1("  MenuTextureSize :      %d\n", MenuTextureSize);
    DEBUGPRINT1("  Shadows :              %s\n", Shadows ? "On" : "Off");
    DEBUGPRINT1("  TextureInterpolation : %s\n", Nearest ? "Off" : "On");
    DEBUGPRINT1("  ScreenXResolution :    %d\n", ScreenXResolution);
    DEBUGPRINT1("  ColorDepth :           %d\n", ColorDepth);
    DEBUGPRINT1("  Reflections :          %s\n", Reflections ? "On" : "Off");
    DEBUGPRINT1("  DisplayFPS :           %s\n", DisplayFPS ? "On" : "Off");
    DEBUGPRINT1("  AmbientLight :         %s\n", AmbientLight ? "On" : "Off");
    DEBUGPRINT1("  LightSources :         %d\n", LightSources);
    DEBUGPRINT1("  FullScreen :           %s\n", FullScreen ? "On" : "Off");
    DEBUGPRINT1("  Language :             %d\n", Language);
    DEBUGPRINT1("  CameraPosition :       %d\n", CameraPosition);
    DEBUGPRINT1("  SolutionHint :         %d\n", SolutionHint);
    DEBUGPRINT1("  MouseSpeed :           %1.2f\n", MouseSpeed);
    DEBUGPRINT1("  SoundVolume :          %d\n", SoundVolume);
    DEBUGPRINT1("  MusicVolume :          %d\n", MusicVolume);
    DEBUGPRINT1("  SoundOn :              %s\n", SoundOn ? "On" : "Off");
    DEBUGPRINT1("  MusicOn :              %s\n", MusicOn ? "On" : "Off");
    DEBUGPRINT1("  SavedGame :            " FMT_UINT64x "\n", SavedGame);
    DEBUGPRINT1("  PlayTime :             %u ms\n", PlayTime);
    DEBUGPRINT1("  Moves :                %u\n", Moves);
    DEBUGPRINT1("  MovesWithHint :        %u\n", MovesWithHint);
    DEBUGPRINT1("  CheatCount :           %u\n", CheatCount);
    DEBUGPRINT1("  PerformanceLog :       %s\n", PerformanceLog ? "On" : "Off");
    DEBUGPRINT1("  SkipProgressBar :      %s\n", SkipProgressBar ?
                                                 "On" : "Off");
}

