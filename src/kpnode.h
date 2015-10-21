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
#include "misc1.h"
#include "kpboard.h"


class KPnode;

class KPnode : public KPboard
{
private:
    KPnode *pchild[MOVES_MAX];
    KPnode *pparent[MOVES_MAX];
    KPnode *pnext;
    mutable short movesToSolve;
    static bool finalizeInProgress;
    static unsigned int count;

    static KPnode *proot;
    static KPnode *pfirst;
    static KPnode *plast;
    static unsigned int LLSize;
    static int    iterationCount;
    static bool   solveCountAvailable;
    static double solveTime;

public:

    KPnode();
    KPnode(const KPnode &src);
    KPnode(const KPboard &src);

    KPnode &operator= (const KPnode &src);
    inline short GetMovesToSolve(void) const
    {
        return movesToSolve;
    };
    inline void  SetMovesToSolve(short m) const
    {
        movesToSolve = m;
    };
    inline bool  AddNextMoves(void);
    inline const KPnode &GetNext(void) const;
    void print(std::ostream &os) const;

    static void finalize();
    static KPnode *GetNodeFor(const KPboard &b);
    static KPnode *GetNodeFor(QWord id);
    static void InitializeRoot(KPnode &n);
    static void LLInitialize(KPnode &n);
    static void LLAddLast(KPnode &n);
    static bool LLIsEmpty(void);
    static void LLSkipFirst(void);
    static unsigned int LLSetFirstToRoot(void);
    static KPnode &LLGetFirst(void);
    static KPnode &LLGetLast(void);
    static unsigned int LLGetSize(void);
    static void CreateSolveTree(KPnode &n);
    static void CalculateSolveCount(void);
    // this is a hack to get a percentage value (1 .. 100) how long it will take
    // to finish the calculation of function CalculateSolveCount()
    static int CalculateSolveCountPercentFinished()
    {
        return iterationCount * 10 / 4333923;
    };
    static void PrintSolveCount(std::ostream &os);
    static int  GetSolutionsCount(void);
    static bool IsSolveCountAvailable(void)
    {
        return solveCountAvailable;
    };
    static double GetSolveTime(void)
    {
        return solveTime;
    };

private:
    int GetNextFreeParentIdx(void) const;
    int GetParentCount(void) const;
    void ModifySolveCount(short i) const;
};

#endif

