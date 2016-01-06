/*
    kpnode.cpp


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

#include "misc1.h"
#ifdef HAVE_UNISTD_H
#include <sys/types.h>
#include <unistd.h>  // needed for sleep
#endif
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <stdexcept>
#include <limits.h>
#include "kpnodes.h"
#include "btime.h"


KPnodes::KPnodes(KPboard &rootBoard) : solveTime(0.0)
{
    CreateSolveTree(rootBoard);
}

KPnodes::~KPnodes()
{
}

void KPnodes::Initialize()
{
    ids.clear();
    nodesForId.clear();
}

KPnode &KPnodes::Add(KPnode &node)
{
    QWord id = node.GetBoard().GetID();

    ids.push_back(id);
    nodesForId[id] = node;

    return nodesForId[id];
}

// Return node for a given board id.
// If id is not available an exception is thrown.
// Before accessing GetNodeFor() it can be checked 
// with Includes() if a board id is available.
KPnode &KPnodes::GetNodeFor(QWord id)
{
    tNodesForId::iterator it;

    if ((it = nodesForId.find(id)) != nodesForId.end())
    {
        return it->second;
    } else {
        throw std::runtime_error("Invalid board id.");
    }
}

void KPnodes::CreateSolveTree(KPboard &rootBoard)
{
    KPnode rootNode(rootBoard);
    int index;

    Initialize();

    Add(rootNode);

    for (index = 0; index < ids.size(); ++index)
    {
        KPnode &node = GetNodeFor(ids[index]);
        if (!node.GetBoard().IsSolved())
        {
            node.AddNextMoves(*this);
        }
        //if (it->IsSolved())
        //  it->print(std::cout);
    }
}

int KPnodes::GetSolutionsCount(void)
{
    tNodesForId::const_iterator it;
    int count = 0;

    for (it = nodesForId.begin(); it != nodesForId.end(); ++it)
    {
        if (it->second.GetBoard().IsSolved())
        {
            ++count;
        }
    }

    return count;
}

void KPnodes::CalculateSolveCount(void)
{
    BTime time;

    time.ResetRelativeTime();
    tIds::const_iterator it;
 
    for (it = ids.begin(); it != ids.end(); ++it)
    {
        KPnode &node = GetNodeFor(*it);

        if (node.GetBoard().IsSolved())
        {
            // recursively modify the solve count
            node.RecursiveUpdateSolveCount(0);
        }
    }

    solveTime = time.GetRelativeTimeUsf(true);

}

void KPnodes::PrintSolveCount(std::ostream &os)
{
    tNodesForId::const_iterator it;

    for (it = nodesForId.begin(); it != nodesForId.end(); ++it)
    {
        os << ' ' << it->second.GetMovesToSolve() << std::endl;
    }
}

