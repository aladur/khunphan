/*
    kpscore.h


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

#ifndef KPSCORE_H
#define KPSCORE_H


#include <stdio.h>
#include <time.h>
#include "misc1.h" // Needed for NULL
#include <string>

#define MAX_SCORE_ENTRIES 10

class KPscore
{
protected:
  typedef struct
  {
    std::string  Name;
    unsigned int PlayTime;
    unsigned int Moves;
    time_t       Timestamp;
  } tKpScoreStruct;
  
public:
  static KPscore &Instance();
  static void finalize() { delete instance; instance = NULL; };

  int  GetMaxEntries() { return MAX_SCORE_ENTRIES; };
  int  GetEntryCount() { return entryCount; };
  void ReadFromFile();
  void WriteToFile();
  void PrintTo(FILE *fp);
  std::string GetFileName();
  void SetFileName(const char *fileName);
  bool Add   (const char *aName, unsigned int aPlayTime, unsigned int aMoves, time_t aTime = 0);
  bool CanAdd(const char *aName, unsigned int aPlayTime, unsigned int aMoves, time_t aTime = 0);
  void ClearAll();
  bool Get(int index, std::string &aName, unsigned int *aPlayTime = NULL, unsigned int *aMoves = NULL, time_t *aTime = NULL);
  // CheckPlayTime = true:  Use PlayTime for the highscore
  // CheckPlayTime = false: Use Moves    for the highscore
  void CheckPlayTime(bool state) { checkPlayTime = state; };

protected: 
  KPscore();
  virtual ~KPscore();
  int PositionToInsert(const char *aName, unsigned int aPlayTime, unsigned int aMoves, time_t aTime = 0);
  std::string fileName;
  int     entryCount;
  bool    checkPlayTime;
  tKpScoreStruct *pScore;

private:
  static KPscore *instance;
};

#endif
