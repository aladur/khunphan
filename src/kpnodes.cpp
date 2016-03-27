/*
    kpnode.cpp


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
#include <stdexcept>
#include <limits.h>
#include "kpnodes.h"
#include "btime.h"


KPnodes::KPnodes(KPnode rootNode) :
    createSolveTreeTime(0.0), calculateSolveCountTime(0.0)
{
    BTime time;

    time.ResetRelativeTime();

    CreateSolveTree(rootNode);

    createSolveTreeTime = time.GetRelativeTimeUsf(true);
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

void KPnodes::CreateSolveTree(KPnode &rootNode)
{
    size_t index;

    Initialize();

    Add(rootNode);

    for (index = 0; index < ids.size(); ++index)
    {
        KPnode &node = GetNodeFor(ids[index]);

        node.AddNextMoves(*this);
    }
}

void KPnodes::CalculateSolveCount(void)
{
    BTime time;

    time.ResetRelativeTime();
    tIds::const_iterator it;
 
    for (it = ids.begin(); it != ids.end(); ++it)
    {
        KPnode &node = GetNodeFor(*it);

        // recursively modify the solve count
        node.RecursiveUpdateSolveCount(0, true);
    }

    calculateSolveCountTime = time.GetRelativeTimeUsf(true);

}

void KPnodes::PrintSolveCount(std::ostream &os) const
{
    tNodesForId::const_iterator it;

    for (it = nodesForId.begin(); it != nodesForId.end(); ++it)
    {
        os << ' ' << it->second.GetMovesToSolve() << std::endl;
    }
}

