/*
    kpnodes.h


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

#ifndef __kpnodes_h__
#define __kpnodes_h__

#include <map>
#include "misc1.h"
#include "kpnode.h"


typedef std::map<QWord, KPnode> tNodesForId;
typedef std::vector<QWord> tIds;

class KPnodes
{
private:
    tIds ids;
    tNodesForId nodesForId;
    double solveTime;

public:
    friend KPnode;

    KPnodes(KPnode rootNode);
    ~KPnodes();

    bool Includes(QWord id)
    {
        return nodesForId.find(id) != nodesForId.end();
    }
    KPnode &GetNodeFor(QWord id);
    unsigned int GetSize(void)
    {
        return nodesForId.size();
    }
    KPnode &GetRootNode(void)
    {
        return nodesForId[*ids.begin()];
    }
    void CalculateSolveCount(void);
    void PrintSolveCount(std::ostream &os) const;
    template<typename F>
    void Iterate(F &f)
    {
        for (size_t index = 0; index < ids.size(); ++index)
        {
            f.Iterate(GetNodeFor(ids[index]));
        }
    }

    double GetSolveTime(void)
    {
        return solveTime;
    };

private:
    void Initialize(void);
    KPnode &Add(KPnode &);
    void CreateSolveTree(KPnode &rootNode);
};

#endif

