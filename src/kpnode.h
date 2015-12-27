/*
    kpnode.h


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

#ifndef __kpnode_h__
#define __kpnode_h__

#include <ostream>
#include <vector>
#include "misc1.h"
#include "kpboard.h"


class KPnodes;

class KPnode
{
    static unsigned int iterations;
    std::vector<KPnode *> childs;   // non-owning pointer
    std::vector<KPnode *> parents;  // non-owning pointer
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

    // this is a hack to get a percentage value (1 .. 100) how long it will take
    // to finish the calculation of function CalculateSolveCount()
    static int CalculateSolveCountPercentFinished()
    {
        return iterations * 10 / 4333923;
    };
    const KPboard &GetBoard() const
    {
        return board;
    }
    void SetBoard(const KPboard &src)
    {
        board = src;
    }
    friend KPnodes;

private:
    void AddNextMoves(KPnodes &nodes);
    void RecursiveUpdateSolveCount(int i);
    inline void SetMovesToSolve(int m)
    {
        movesToSolve = m;
    };
    static inline void ClearIterationCount(void)
    {
        iterations = 0;
    }
    KPboard board;
};

#endif

