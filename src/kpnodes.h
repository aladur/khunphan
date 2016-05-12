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

#ifndef KPNODES_H
#define KPNODES_H


#include "stdafx.h"
#include <unordered_map>
#include "kpnode.h"


typedef std::unordered_map<QWord, KPnode> tNodesForId;
typedef std::vector<QWord> tIds;

class KPnodes
{
public:
    friend class KPnode;

    KPnodes(KPnode rootNode);

    bool Includes(QWord id) const
    {
        return nodesForId.find(id) != nodesForId.end();
    }
    KPnode &GetNodeFor(QWord id);
    const KPnode &GetNodeFor(QWord id) const;
    size_t GetSize(void)
    {
        return nodesForId.size();
    }
    KPnode &GetRootNode(void)
    {
        return nodesForId[*ids.begin()];
    }
    void CalculateSolveCount(void);
    void PrintSolveCount(std::ostream &os) const;
    unsigned int GetSolutionsCount();

    double GetCalculateSolveCountTime(void) const
    {
        return calculateSolveCountTime;
    };
    double GetCreateSolveTreeTime(void) const
    {
        return createSolveTreeTime;
    };

private:
    void Initialize(void);
    KPnode &Add(KPnode &);
    void CreateSolveTree(KPnode &rootNode);

    tIds ids;
    tNodesForId nodesForId;
    double createSolveTreeTime;
    double calculateSolveCountTime;
    bool isReadOnly;
};

#endif

