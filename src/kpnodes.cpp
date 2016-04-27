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
    createSolveTreeTime(0.0), calculateSolveCountTime(0.0), isReadOnly(false)
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
    isReadOnly = false;
}

KPnode &KPnodes::Add(KPnode &node)
{
    if (isReadOnly)
    {
        throw std::logic_error(
            "Calling Add is not allowed in read-only mode.");
    }

    auto id = node.GetBoard().GetID();

    ids.emplace_back(id);
    nodesForId.emplace(id, node);

    return nodesForId[id];
}

// Return node for a given board id.
// If id is not available an exception is thrown.
// Before accessing GetNodeFor() it can be checked
// with Includes() if a board id is available.
const KPnode &KPnodes::GetNodeFor(QWord id) const
{
    auto it = nodesForId.find(id);

    if (it != nodesForId.end())
    {
        return it->second;
    }
    else
    {
        throw std::runtime_error("Invalid board id.");
    }
}

KPnode &KPnodes::GetNodeFor(QWord id)
{
    auto it = nodesForId.find(id);

    if (it != nodesForId.end())
    {
        return it->second;
    }
    else
    {
        throw std::runtime_error("Invalid board id.");
    }
}

void KPnodes::CreateSolveTree(KPnode &rootNode)
{
    decltype(ids.size()) index;

    if (isReadOnly)
    {
        throw std::logic_error(
            "Calling CreateSolveTree is not allowed in read-only mode.");
    }

    Initialize();

    Add(rootNode);

    // This for loop must be executed by an index (and not an
    // iterator or range based for).
    // Reason: Within the loop AddNextMoves adds new elements to the end
    // of ids vector.
    for (index = 0; index < ids.size(); ++index)
    {
        auto &node = GetNodeFor(ids[index]);

        node.AddNextMoves(*this);
    }

    // Switch to read-only mode. After this point no nodes can be added
    // any more.
    // Reason: The node elements store pointers to other node elements
    // in the childs and parents members. This only works if the vector
    // is not changed any more.
    isReadOnly = true;
}

void KPnodes::CalculateSolveCount(void)
{
    BTime time;

    time.ResetRelativeTime();

    for (const auto id : ids)
    {
        auto &node = GetNodeFor(id);

        // recursively modify the solve count
        node.RecursiveUpdateSolveCount(0, true);
    }

    calculateSolveCountTime = time.GetRelativeTimeUsf(true);

}

void KPnodes::PrintSolveCount(std::ostream &os) const
{
    for (const auto id : ids)
    {
        const auto &node = GetNodeFor(id);

        os << ' ' << node.GetMovesToSolve() << std::endl;
    }
}

