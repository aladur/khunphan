/*
    kpscore.h


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

#ifndef KPSCORE_H
#define KPSCORE_H


#include "stdafx.h"
#include <vector>
#include <ostream>
#include <time.h>
#include <string>


class KPscore
{
public:
    typedef struct
    {
        std::string  Name;
        unsigned int PlayTime;
        unsigned int Moves;
        time_t       Timestamp;
    } tKpScoreStruct;

    KPscore(const char *fileName = NULL);

    unsigned int GetMaxEntries() const
    {
        return MAX_SCORE_ENTRIES;
    }
    unsigned int GetEntryCount() const
    {
        return scoreList.size();
    }
    std::string GetFileName() const
    {
        return fileName;
    }
    std::string GetFileVersion() const
    {
        return fileVersion;
    }
    void WriteToFile() const;
    void print(std::ostream &os) const;
    void SetFileName(const char *fileName = NULL);
    bool Add(const char *aName, unsigned int aPlayTime, unsigned int aMoves,
             time_t aTime = 0);
    bool CanAdd(const char *aName, unsigned int aPlayTime, unsigned int aMoves,
                time_t aTime = 0) const;
    void ClearAll();
    bool Get(unsigned int index, std::string &aName,
             unsigned int *aPlayTime = NULL,
             unsigned int *aMoves = NULL, time_t *aTime = NULL) const;

private:
    void ReadFromFile();
    std::vector<KPscore::tKpScoreStruct>::iterator PositionToInsert(
                         const char *aName, unsigned int aPlayTime,
                         unsigned int aMoves, time_t aTime = 0);
    std::vector<KPscore::tKpScoreStruct>::const_iterator PositionToInsert(
                         const char *aName, unsigned int aPlayTime,
                         unsigned int aMoves, time_t aTime = 0) const;

    std::string fileName;
    std::string fileVersion;
    std::vector<tKpScoreStruct> scoreList;

    static const unsigned int MAX_SCORE_ENTRIES;
};

#endif
