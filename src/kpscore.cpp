/*
    kpscore.cpp


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
#include <sstream>
#include <libxml/tree.h>
#include <sys/stat.h>
#include "kpscore.h"
#ifdef WIN32
    #include <shlwapi.h>
    #pragma comment(lib,"shlwapi.lib")
    #include "shlobj.h"
#endif

#define _TO(type)   reinterpret_cast<const xmlChar *>(type)
#define _FROM(type) reinterpret_cast<const char *>(type)

const unsigned int KPscore::MAX_SCORE_ENTRIES = 10;

KPscore::KPscore(const char *fileName /* = NULL */)
{
    SetFileName(fileName);
    ReadFromFile();
}

////////////////////////////////////////////////////////////////////////////////
// Score interface
////////////////////////////////////////////////////////////////////////////////

bool KPscore::Add(const char *aName, unsigned int aPlayTime,
                  unsigned int aMoves, time_t aTimestamp)
{
    if (CanAdd(aName, aPlayTime, aMoves, aTimestamp))
    {
        tKpScoreStruct newItem;

        newItem.Name      = aName;
        newItem.PlayTime  = aPlayTime;
        newItem.Moves     = aMoves;
        newItem.Timestamp = (aTimestamp != 0) ? aTimestamp : time(NULL);

        std::vector<tKpScoreStruct>::iterator it =
            PositionToInsert(aName, aPlayTime, aMoves, aTimestamp);

        scoreList.insert(it, newItem);

        if (scoreList.size() > MAX_SCORE_ENTRIES)
        {
            // erase last element
            it = scoreList.end();
            --it;
            scoreList.erase(it);
        }

        return true;
    }

    return false;
}

bool KPscore::CanAdd(const char *aName, unsigned int aPlayTime,
                     unsigned int aMoves, time_t aTimestamp) const
{
    std::vector<tKpScoreStruct>::const_iterator it =
        PositionToInsert(aName, aPlayTime, aMoves, aTimestamp);

    return (it != scoreList.end() || scoreList.size() < MAX_SCORE_ENTRIES);
}

void KPscore::ClearAll()
{
    scoreList.clear();
    fileVersion = "";
}

bool KPscore::Get(unsigned int index, std::string &pName,
                  unsigned int *pPlayTime,
                  unsigned int *pMoves, time_t *pTimestamp) const
{
    if (index >= scoreList.size())
    {
        return false;
    }

    pName = scoreList[index].Name;

    if (pPlayTime  != NULL)
    {
        *pPlayTime = scoreList[index].PlayTime;
    }

    if (pMoves != NULL)
    {
        *pMoves = scoreList[index].Moves;
    }

    if (pTimestamp != NULL)
    {
        *pTimestamp = scoreList[index].Timestamp;
    }

    return true;
}

std::vector<KPscore::tKpScoreStruct>::iterator KPscore::PositionToInsert(
    const char *, unsigned int aPlayTime,
    unsigned int, time_t)
{
    std::vector<tKpScoreStruct>::iterator it = scoreList.begin();

    for (; it != scoreList.end(); ++it)
    {
        if (aPlayTime < it->PlayTime)
        {
            break;
        }
    }

    return it;
}

std::vector<KPscore::tKpScoreStruct>::const_iterator KPscore::PositionToInsert(
    const char *, unsigned int aPlayTime,
    unsigned int, time_t) const
{
    std::vector<tKpScoreStruct>::const_iterator it = scoreList.begin();

    for (; it != scoreList.end(); ++it)
    {
        if (aPlayTime < it->PlayTime)
        {
            break;
        }
    }

    return it;
}

////////////////////////////////////////////////////////////////////////////////
// File interface
////////////////////////////////////////////////////////////////////////////////

// if aFileName is NULL initialize it to the default
void KPscore::SetFileName(const char *aFileName /* = NULL */)
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
#else
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
#else
#error "Unsupported platform"
#endif
#endif
    }
}

void KPscore::WriteToFile() const
{
    if (scoreList.empty())
    {
        return;
    }

    xmlNsPtr ns = NULL;
    xmlDocPtr doc = xmlNewDoc(_TO("1.0"));
    doc->children = xmlNewDocNode(doc, ns, _TO("KhunPhan"), NULL);
    xmlSetProp(doc->children, _TO("Version"), _TO(VERSION));

    xmlNodePtr tree = xmlNewChild(doc->children, ns, _TO("Scores"), NULL);

    std::vector<tKpScoreStruct>::const_iterator it = scoreList.begin();

    for (; it != scoreList.end(); ++it)
    {
        xmlNodePtr subtree = xmlNewChild(tree, ns, _TO("Score"), NULL);

        xmlNewTextChild(subtree, ns, _TO("Name"),
                        _TO(it->Name.c_str()));

        std::ostringstream iss1;
        iss1 << it->PlayTime;
        xmlNewChild(subtree, ns, _TO("PlayTime"), _TO(iss1.str().c_str()));

        std::ostringstream iss2;
        iss2 << it->Moves;
        xmlNewChild(subtree, ns, _TO("Moves"), _TO(iss2.str().c_str()));

        std::ostringstream iss3;
        iss3 << it->Timestamp;
        xmlNewChild(subtree, ns, _TO("Timestamp"), _TO(iss3.str().c_str()));
    }

    xmlSaveFormatFile(GetFileName().c_str(), doc, 1);

    xmlFreeDoc(doc);
}

void KPscore::ReadFromFile()
{
    struct stat statbuf;

    ClearAll();

    if (stat(GetFileName().c_str(), &statbuf) != 0)
    {
        return;
    }

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

                    if (key)
                    {
                        Name = _FROM(key);
                        xmlFree(key);
                    }
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

void KPscore::print(std::ostream &os) const
{
#ifndef WIN32

    if (scoreList.empty())
    {
        os << "Current KhunPhan Score list is empty" << std::endl;
    }
    else
    {
        os << "Current KhunPhan Score list:" << std::endl;
        std::vector<tKpScoreStruct>::const_iterator it = scoreList.begin();

        for (; it != scoreList.end(); ++it)
        {
            os << "   Name: '" << it->Name
               << "' PlayTime: " << it->PlayTime
               << " ms Moves: " << it->Moves
               << " Time: " << ctime(&it->Timestamp) ;
        }

        os << std::endl;
    }

#endif
}
