/*
    kpnode.cpp


    Copyright (C) 2001-2018  W. Schwotzer

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
#include <limits.h>
#include "kpnode.h"
#include "kpnodes.h"

//#define DEBUG_OUTPUT 1


KPnode::KPnode() : movesToSolve(SHRT_MAX)
{
}

KPnode::KPnode(const KPnode &src) :
    childs(src.childs), parents(src.parents), movesToSolve(SHRT_MAX),
    board(src.board)
{
    movesToSolve = src.GetMovesToSolve();
}

KPnode::KPnode(const KPboard &src) : movesToSolve(SHRT_MAX), board(src)
{
}

KPnode &KPnode::operator= (const KPnode &src)
{
    if (&src != this)
    {
        board = src.board;
        movesToSolve = src.movesToSolve;
        childs = src.childs;
        parents = src.parents;
    }

    return *this;
}

void KPnode::AddNextMoves(KPnodes &nodes)
{
    if (!GetBoard().IsSolved() && childs.empty())
    {
        auto moves = board.GetPossibleMoves();

        for (auto &move : moves)
        {
            auto boardMoved = board;

            boardMoved.Move(move);

            if (!nodes.Includes(boardMoved.GetID()))
            {
                KPnode node(boardMoved);

                // Position not found in tree: Add a new node
                // Important: Get the added reference in the nodes container
                // and push it into the childs vector
                auto &newNode = nodes.Add(node);
#ifdef DEBUG_OUTPUT
                std::cout << nodes.GetSize() << ". New: " << std::endl;
                newNode.print(std::cout);
#endif
                // Create parent and child links to existing node
                childs.push_back(newNode);
                newNode.parents.push_back(*this);
            }
            else
            {
                // Position already in tree
                // Important: Get the existing reference in the nodes container
                // and push it into the childs vector
                auto &existingNode = nodes.GetNodeFor(boardMoved.GetID());
                // Create parent and child links to existing node
                childs.push_back(existingNode);
                existingNode.parents.push_back(*this);
            }
        }
    }
}

void KPnode::print(std::ostream &os, bool with_childs /* = false */) const
{
    os << "Node:" << std::endl;
    GetBoard().print(os);

    if (with_childs)
    {
        auto i = 1;

        for (auto &child : childs)
        {
            os << i << ". Child:" << std::endl;
            child.get().print(os);
            ++i;
        }
    }

    os << std::endl;
}

void KPnode::RecursiveUpdateSolveCount(int count, bool start)
{
    // Every game position can be solved in less or equal than 126 moves.
    // => For higher values abort the recursion.
    // In addition only recurse if solve count is lower than the
    // already estimated solve count.
    // This tremendously reduces the calculation time.
    if ((start && (GetBoard().IsSolved())) ||
        (!start && (count <= 126) && (count < GetMovesToSolve())))
    {
        movesToSolve = count;

        for (auto &parent : parents)
        {
            parent.get().RecursiveUpdateSolveCount(count + 1, false);
        }
    }
}
