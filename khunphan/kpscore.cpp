/*
    kpscore.cpp


    Automatic solution finder for KhunPhan game
    Copyright (C) 2001,2002,2003  W. Schwotzer

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

#include <stdio.h>
#include <libxml/tree.h>
#include "misc1.h"
#include "kpscore.h"
#include "config.h"


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

//////////////////////////////////////////////////////////////////////////////////////////
// Score interface
//////////////////////////////////////////////////////////////////////////////////////////

bool KPscore::Add(const char *aName, unsigned int aPlayTime, unsigned int aMoves, time_t aTimestamp)
{
  int i, j;

  i = PositionToInsert(aName, aPlayTime, aMoves, aTimestamp);
  if (i < 0 || i >= GetMaxEntries())
    return false;

  j = (GetEntryCount() >= GetMaxEntries()) ? GetEntryCount() - 2 : GetEntryCount() - 1;
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
    entryCount++;

  return true;
}

bool KPscore::CanAdd(const char *aName, unsigned int aPlayTime, unsigned int aMoves, time_t aTimestamp)
{
  if (GetEntryCount() >= GetMaxEntries())
    return false;

  int i = PositionToInsert(aName, aPlayTime, aMoves, aTimestamp);
  return (i >= 0 && i < GetMaxEntries());
}

void KPscore::ClearAll()
{
  entryCount = 0;
}

bool KPscore::Get(int i, BString &pName, unsigned int *pPlayTime, unsigned int *pMoves, time_t *pTimestamp)
{
  if (i < 0 || i >= GetEntryCount())
    return false;

  pName                               = pScore[i].Name;
  if (pPlayTime  != NULL) *pPlayTime  = pScore[i].PlayTime;
  if (pMoves     != NULL) *pMoves     = pScore[i].Moves;
  if (pTimestamp != NULL) *pTimestamp = pScore[i].Timestamp;

  return true;  
}

int KPscore::PositionToInsert(const char *, unsigned int aPlayTime, unsigned int aMoves, time_t)
{
  if ( GetEntryCount() <= 0)
    return 0;

  int i;

  for (i = 0; i < GetEntryCount(); i++)
  {
    if (!checkPlayTime && (aMoves    < pScore[i].Moves))
      break;
    if (checkPlayTime  && (aPlayTime < pScore[i].PlayTime))
      break;
  }

  return i;
}

//////////////////////////////////////////////////////////////////////////////////////////
// File interface
//////////////////////////////////////////////////////////////////////////////////////////

BString KPscore::GetFileName()
{
  // Lazy initialization
  if ( fileName.length() == 0 )
    SetFileName( NULL );

  return fileName;
}

// if aFileName is NULL initialize it to the default
void KPscore::SetFileName(const char *aFileName)
{
  if (aFileName != NULL)
    fileName = aFileName;
  else
  {
#ifdef LINUX
    fileName.printf("%s/.KhunPhanScores.xml", getenv("HOME"));
#endif
#ifdef WIN32
    fileName.printf("KhunPhanScores.xml");
#endif
  }
}

void KPscore::WriteToFile()
{
  if (GetEntryCount() <= 0)
    return;

  xmlDocPtr  doc;
  xmlNodePtr tree, subtree, subtree1;
  xmlChar    buffer[128];
  int        i;

  doc = xmlNewDoc((xmlChar *)"1.0");
  doc->children = xmlNewDocNode( doc, NULL, (xmlChar *)"KhunPhan", NULL );
  xmlSetProp( doc->children, (xmlChar *)"Version", (xmlChar *)VERSION );

    // Scores
    tree = xmlNewChild( doc->children, NULL, (xmlChar *)"Scores", NULL );
    for (i = 0; i < GetEntryCount(); i++)
    {
      // Score
      subtree  = xmlNewChild( tree, NULL, (xmlChar *)"Score", NULL );
      sprintf((char *)buffer, "%s", (const char *)pScore[i].Name); subtree1 = xmlNewChild(subtree, NULL, (xmlChar *)"Name",      buffer );
      sprintf((char *)buffer, "%u", pScore[i].PlayTime);           subtree1 = xmlNewChild(subtree, NULL, (xmlChar *)"PlayTime",  buffer );
      sprintf((char *)buffer, "%u", pScore[i].Moves);              subtree1 = xmlNewChild(subtree, NULL, (xmlChar *)"Moves",     buffer );
      sprintf((char *)buffer, "%d", (int)pScore[i].Timestamp);     subtree1 = xmlNewChild(subtree, NULL, (xmlChar *)"Timestamp", buffer );
    }

  xmlSaveFile ((const char *)GetFileName(), doc);
}

void KPscore::ReadFromFile()
{
  ClearAll();

  xmlDocPtr  doc;
  xmlNodePtr cur, tree, subtree, subtree1;
  xmlNsPtr   ns = NULL;
  xmlChar    *p;

  BString      Name;
  unsigned int PlayTime;
  unsigned int Moves;
  time_t       Timestamp;

  if ((doc = xmlParseFile( GetFileName() )) != NULL) {
    cur = doc->xmlChildrenNode;
    if (cur != NULL && (!strcmp((char *)cur->name, "KhunPhan"))) {
      tree = cur->xmlChildrenNode;
      while (tree != NULL) {
        if ((!strcmp((char *)tree->name, "Scores")) && (cur->ns == ns)) {
           subtree = tree->xmlChildrenNode;
           while (subtree != NULL) {
              if ((!strcmp((char *)subtree->name, "Score")) && (cur->ns == ns)) {
                 subtree1 = subtree->xmlChildrenNode;
                 Name = ""; PlayTime = 0; Moves = 0; Timestamp = 0;
                 while (subtree1 != NULL) {
                    if ((!strcmp((char *)subtree1->name, "Name")) && (cur->ns == ns) &&
                       (p = xmlNodeListGetString(doc, subtree1->xmlChildrenNode, 1))) {
                       Name.printf("%s", (const char *)p);
                    }
                    if ((!strcmp((char *)subtree1->name, "PlayTime")) && (cur->ns == ns) &&
                       (p = xmlNodeListGetString(doc, subtree1->xmlChildrenNode, 1))) {
                       PlayTime = strtoul((char *)p, NULL, 10);
                    }
                    if ((!strcmp((char *)subtree1->name, "Moves")) && (cur->ns == ns) &&
                       (p = xmlNodeListGetString(doc, subtree1->xmlChildrenNode, 1))) {
                       Moves = strtoul((char *)p, NULL, 10);
                    }
                    if ((!strcmp((char *)subtree1->name, "Timestamp")) && (cur->ns == ns) &&
                       (p = xmlNodeListGetString(doc, subtree1->xmlChildrenNode, 1))) {
                       Timestamp = strtol((char *)p, NULL, 10);
                    }
                    subtree1 = subtree1->next;
                 } // while
                 Add(Name, PlayTime, Moves, Timestamp);
              }    // if subtree "Scores"
              subtree = subtree->next;
           } // while
        }    // if subtree "Scores"
        tree = tree->next;
      } // while
    }   // if subtree "KhunPhan"
  } // if
}

void KPscore::PrintTo(FILE *fp)
{
#ifndef WIN32
  if (GetEntryCount() <= 0)
    fprintf(fp, "Current KhunPhan Score list is empty\n");
  else
  {
    fprintf(fp, "Current KhunPhan Score list:\n");
    for (int i = 0; i < GetEntryCount(); i++)
    {
      fprintf(fp, "   Name: '%s' PlayTime: %d ms Moves: %d Time: %s",
         (const char *)pScore[i].Name, pScore[i].PlayTime, pScore[i].Moves, ctime(&pScore[i].Timestamp));
    }
    fprintf(fp, "\n");
  }
#endif
}
