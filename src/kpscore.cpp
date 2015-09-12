/*
    kpscore.cpp


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

#include <sstream>
#include <libxml/tree.h>
#include "misc1.h"
#include "kpscore.h"
#ifdef WIN32
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#include "shlobj.h"
#endif

#define _TO(type)   reinterpret_cast<const xmlChar *>(type)
#define _FROM(type) reinterpret_cast<const char *>(type)

KPscore *KPscore::instance = NULL;

KPscore::KPscore() : entryCount(0), checkPlayTime(true), pScore(NULL)
{
    pScore = new tKpScoreStruct [GetMaxEntries()];
}

KPscore::~KPscore()
{
    delete [] pScore;
}

KPscore &KPscore::Instance()
{
    if (instance == NULL)
    {
        instance = new KPscore;
        atexit ( KPscore::finalize );
    };
    return *instance;
}

////////////////////////////////////////////////////////////////////////////////
// Score interface
////////////////////////////////////////////////////////////////////////////////

bool KPscore::Add(const char *aName, unsigned int aPlayTime,
                  unsigned int aMoves, time_t aTimestamp)
{
    int i, j;

    i = PositionToInsert(aName, aPlayTime, aMoves, aTimestamp);
    if (i < 0 || i >= GetMaxEntries())
    {
        return false;
    }

    j = (GetEntryCount() >= GetMaxEntries()) ? GetEntryCount() - 2 :
        GetEntryCount() - 1;

    while ( j >= i)
    {
        pScore[j+1].Name      = pScore[j].Name;
        pScore[j+1].PlayTime  = pScore[j].PlayTime;
        pScore[j+1].Moves     = pScore[j].Moves;
        pScore[j+1].Timestamp = pScore[j].Timestamp;
        j--;
    }
    pScore[i].Name      = aName;
    pScore[i].PlayTime  = aPlayTime;
    pScore[i].Moves     = aMoves;
    pScore[i].Timestamp = (aTimestamp != 0) ? aTimestamp : time(NULL);

    if (GetEntryCount() < GetMaxEntries())
    {
        entryCount++;
    }

    return true;
}

bool KPscore::CanAdd(const char *aName, unsigned int aPlayTime,
                     unsigned int aMoves, time_t aTimestamp)
{
    if (GetEntryCount() >= GetMaxEntries())
    {
        return false;
    }

    int i = PositionToInsert(aName, aPlayTime, aMoves, aTimestamp);
    return (i >= 0 && i < GetMaxEntries());
}

void KPscore::ClearAll()
{
    entryCount  = 0;
    fileVersion = "";
}

bool KPscore::Get(int i, std::string &pName, unsigned int *pPlayTime,
                  unsigned int *pMoves, time_t *pTimestamp)
{
    if (i < 0 || i >= GetEntryCount())
    {
        return false;
    }

    pName           = pScore[i].Name;
    if (pPlayTime  != NULL)
    {
        *pPlayTime  = pScore[i].PlayTime;
    }
    if (pMoves     != NULL)
    {
        *pMoves     = pScore[i].Moves;
    }
    if (pTimestamp != NULL)
    {
        *pTimestamp = pScore[i].Timestamp;
    }

    return true;
}

int KPscore::PositionToInsert(const char *, unsigned int aPlayTime,
                              unsigned int aMoves, time_t)
{
    if ( GetEntryCount() <= 0)
    {
        return 0;
    }

    int i;

    for (i = 0; i < GetEntryCount(); i++)
    {
        if (!checkPlayTime && (aMoves    < pScore[i].Moves))
        {
            break;
        }
        if (checkPlayTime  && (aPlayTime < pScore[i].PlayTime))
        {
            break;
        }
    }

    return i;
}

////////////////////////////////////////////////////////////////////////////////
// File interface
////////////////////////////////////////////////////////////////////////////////

std::string KPscore::GetFileName()
{
    // Lazy initialization
    if ( fileName.length() == 0 )
    {
        SetFileName( NULL );
    }

    return fileName;
}

// if aFileName is NULL initialize it to the default
void KPscore::SetFileName(const char *aFileName)
{
    if (aFileName != NULL)
    {
        fileName = aFileName;
    }
    else
    {
#ifdef LINUX
        fileName = getenv("HOME");
        fileName += "/.KhunPhanScores.xml";
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
        fileName += "KhunPhanScores.xml";
#endif
    }
}

void KPscore::WriteToFile()
{
    if (GetEntryCount() <= 0)
    {
        return;
    }

    xmlNsPtr ns = NULL;
    xmlDocPtr doc = xmlNewDoc(_TO("1.0"));
    doc->children = xmlNewDocNode(doc, ns, _TO("KhunPhan"), NULL);
    xmlSetProp(doc->children, _TO("Version"), _TO(VERSION));

    xmlNodePtr tree = xmlNewChild(doc->children, ns, _TO("Scores"), NULL);

    for (int i = 0; i < GetEntryCount(); i++)
    {
        xmlNodePtr subtree = xmlNewChild(tree, ns, _TO("Score"), NULL);

        xmlNewTextChild(subtree, ns, _TO("Name"), _TO(pScore[i].Name.c_str()));

        std::ostringstream iss1;
        iss1 << pScore[i].PlayTime;
        xmlNewChild(subtree, ns, _TO("PlayTime"), _TO(iss1.str().c_str()));

        std::ostringstream iss2;
        iss2 << pScore[i].Moves;
        xmlNewChild(subtree, ns, _TO("Moves"), _TO(iss2.str().c_str()));

        std::ostringstream iss3;
        iss3 << pScore[i].Timestamp;
        xmlNewChild(subtree, ns, _TO("Timestamp"), _TO(iss3.str().c_str()));
    }

    xmlSaveFormatFile(GetFileName().c_str(), doc, 1);

    xmlFreeDoc(doc);
}

void KPscore::ReadFromFile()
{
    ClearAll();

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
        if (xmlStrcmp(tree->name, _TO("Scores")))
        {
            tree = tree->next;
            continue;
        }

        xmlNodePtr subtree = tree->xmlChildrenNode;

        while (subtree != NULL)
        {
            std::string  Name;
            time_t       Timestamp = 0;
            unsigned int PlayTime  = 0;
            unsigned int Moves     = 0;

            if (xmlStrcmp(subtree->name, _TO("Score")))
            {
                subtree = subtree->next;
                continue;
            }

            xmlNodePtr subtree1 = subtree->xmlChildrenNode;

            while (subtree1 != NULL)
            {
                const xmlChar *tag = subtree1->name;
                xmlNodePtr node    = subtree1->xmlChildrenNode;
                xmlChar *key;

                if (!xmlStrcmp(tag, _TO("Name")))
                {
                    key  = xmlNodeListGetString(doc, node, 1);
                    Name = _FROM(key);
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
                else if (!xmlStrcmp(tag, _TO("Timestamp")))
                {
                    key = xmlNodeListGetString(doc, node, 1);
                    std::istringstream iss(_FROM(key));
                    iss >> Timestamp;
                    xmlFree(key);
                }

                subtree1 = subtree1->next;
            }

            Add(Name.c_str(), PlayTime, Moves, Timestamp);
            subtree = subtree->next;
        }

        tree = tree->next;
    }
}

void KPscore::PrintTo(FILE *fp)
{
#ifndef WIN32
    if (GetEntryCount() <= 0)
    {
        fprintf(fp, "Current KhunPhan Score list is empty\n");
    }
    else
    {
        fprintf(fp, "Current KhunPhan Score list:\n");
        for (int i = 0; i < GetEntryCount(); i++)
        {
            fprintf(fp, "   Name: '%s' PlayTime: %d ms Moves: %d Time: %s",
                    pScore[i].Name.c_str(), pScore[i].PlayTime, pScore[i].Moves,
                    ctime(&pScore[i].Timestamp));
        }
        fprintf(fp, "\n");
    }
#endif
}
