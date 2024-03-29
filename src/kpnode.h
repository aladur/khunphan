/*
    kpnode.h


    Copyright (C) 2001-2024  W. Schwotzer

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

#ifndef KPNODE_H
#define KPNODE_H

#include "stdafx.h"
#include <ostream>
#include <vector>
#include <functional>
#include "kpboard.h"


class KPnodes;

class KPnode
{
    // Use reference_wrapper to store references to existing nodes
    // in the childs or parents vector.
    std::vector<std::reference_wrapper<KPnode> > childs;
    std::vector<std::reference_wrapper<KPnode> > parents;
    int movesToSolve;

public:
    KPnode();
    KPnode(const KPnode &src);
    KPnode(const KPboard &src);

    KPnode &operator= (const KPnode &src);
    inline int GetMovesToSolve(void) const
    {
        return movesToSolve;
    };
    void print(std::ostream &os, bool with_childs = false) const;

    const KPboard &GetBoard() const
    {
        return board;
    }
    void SetBoard(const KPboard &src)
    {
        board = src;
    }
    friend class KPnodes;

private:
    void AddNextMoves(KPnodes &nodes);
    void RecursiveUpdateSolveCount(int count, bool start);
    inline void SetMovesToSolve(int m)
    {
        movesToSolve = m;
    };
    KPboard board;
};

#endif

