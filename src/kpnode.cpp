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
#include <limits.h>
#include "kpnode.h"
#include "kpnodes.h"

//#define DEBUG_OUTPUT 1


KPnode::KPnode() : movesToSolve(SHRT_MAX)
{
}

KPnode::KPnode(const KPnode &src) : board(src.board),
    childs(src.childs), parents(src.parents), movesToSolve(SHRT_MAX)
{
    movesToSolve = src.GetMovesToSolve();
}

KPnode::KPnode(const KPboard &src) : board(src),
    movesToSolve(SHRT_MAX)
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
    KPnode *pnode;
    tKPTokenID id = TK_GREEN1;

    if (!GetBoard().IsSolved() && childs.empty())
    {
        do
        {
            tKPDirection direction;

            direction = MOVE_UP;
            do
            {
                KPboard boardMoved(board);

                if (boardMoved.Move(id, direction))
                {
                    if (!nodes.Includes(boardMoved.GetID()))
                    {
                        KPnode node(boardMoved);

                        pnode = &nodes.Add(node);
#ifdef DEBUG_OUTPUT
                        std::cout << nodes.GetSize() << ". New: " << std::endl;
                        pnode->print(std::cout);
#endif
                    }
                    else
                    {
                        // position already in tree
                        // Create parent and child links to existing node
                        pnode = &nodes.GetNodeFor(boardMoved.GetID());
                    }
                    childs.push_back(pnode);
                    pnode->parents.push_back(this);
                } // if
            }
            while (++direction != MOVE_UP);
        }
        while (++id != TK_GREEN1);
    }
}

void KPnode::print(std::ostream &os, bool with_childs /* = false */) const
{
    os << "Node:" << std::endl;
    GetBoard().print(os);

    if (with_childs)
    {
        std::vector<KPnode *>::const_iterator it;
        int i = 1;

        for (it = childs.begin(); it != childs.end(); ++it)
        {
            os << i << ". Child:" << std::endl;
            (*it)->print(os);
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
        std::vector<KPnode *>::iterator it;

        movesToSolve = count;

        for (it = parents.begin(); it != parents.end(); ++it)
        {
            (*it)->RecursiveUpdateSolveCount(count + 1, false);
        }
    }
}
