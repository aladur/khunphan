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
#include "kpnode.h"
#include "btime.h"


KPnode *KPnode::proot  = NULL;
KPnode *KPnode::pfirst = NULL;
KPnode *KPnode::plast  = NULL;
unsigned int KPnode::LLSize = 0;
bool KPnode::solveCountAvailable = false;
int  KPnode::iterationCount = 0;
bool KPnode::finalizeInProgress = false;
unsigned int KPnode::count = 0;
double KPnode::solveTime = 0.0;

KPnode::KPnode() : pnext(NULL), movesToSolve(SHRT_MAX)
{
}

KPnode::KPnode(const KPnode &src) : KPboard (src), pnext(NULL),
    childs(src.childs), parents(src.parents), movesToSolve(SHRT_MAX)
{
    movesToSolve = src.GetMovesToSolve();
}

KPnode::KPnode(const KPboard &src) : KPboard (src), pnext(NULL),
    movesToSolve(SHRT_MAX)
{
}

KPnode &KPnode::operator= (const KPnode &src)
{
    if (&src != this)
    {
        KPboard::operator= (src);

        pnext = NULL;
        movesToSolve = src.movesToSolve;
        childs = src.childs;
        parents = src.parents;
    }

    return *this;
}

inline bool KPnode::AddNextMoves()
{
    KPnode *pn;
    tKPTokenID id = TK_GREEN1;

    do
    {
        tKPDirection direction;

        direction = MOVE_UP;
        do
        {
            KPboard board(*this);

            if (board.Move(id, direction))
            {
                if (!board.IsMemberOf())
                {
                    ++count;
                    pn = new KPnode(board);
                    KPnode::LLAddLast(*pn);
                    KPboard::idHash.Add(LLGetLast().GetID(), &LLGetLast());
                    pn = &LLGetLast();
#ifdef DEBUG_OUTPUT
                    std::cout << count << ". New: " << std::endl;
                    LLGetLast().print(std::cout);
#endif
                }
                else
                {
                    // position already in tree
                    // Create parent and child links to existing node
                    pn = GetNodeFor(board);
                }
                childs.push_back(pn);
                pn->parents.push_back(this);
            } // if
        }
        while (++direction != MOVE_UP);
    }
    while (++id != TK_GREEN1);
    return true;
}

void KPnode::print(std::ostream &os, bool with_childs /* = false */) const
{
    os << "Node:" << std::endl;
    KPboard::print(os);

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

KPnode *KPnode::GetNodeFor(const KPboard &b)
{
    return static_cast<KPnode *>(KPboard::idHash.GetObjectWith(b.GetID()));
}

KPnode *KPnode::GetNodeFor(QWord id)
{
    return static_cast<KPnode *>(KPboard::idHash.GetObjectWith(id));
}

inline const KPnode &KPnode::GetNext(void) const
{
    return *pnext;
}

void KPnode::InitializeRoot(KPnode &n)
{
    // add root object to hash table
    KPboard::idHash.Add(n.GetID(), &n);
    proot = &n;
    atexit( KPnode::finalize );
}

void KPnode::LLInitialize(KPnode &n)
{
    pfirst = &n;
    plast  = &n;
    LLSize = 1;
}

void KPnode::LLAddLast(KPnode &n)
{
    plast->pnext  = &n;
    plast = &n;
    LLSize++;
}

void KPnode::LLSkipFirst(void)
{
    pfirst = pfirst->pnext;
    LLSize--;
}

KPnode &KPnode::LLGetFirst(void)
{
    return *pfirst;
}

KPnode &KPnode::LLGetLast(void)
{
    return *plast;
}
bool KPnode::LLIsEmpty(void)
{
    return (pfirst == NULL);
}

unsigned int KPnode::LLGetSize(void)
{
    return LLSize;
}

unsigned int KPnode::LLSetFirstToRoot(void)
{
    KPnode *p;

    LLSize = 1;
    p = pfirst = proot;
    while (p != plast)
    {
        p = p->pnext;
        LLSize++;
    }
    return LLSize;
}

void KPnode::CreateSolveTree(KPnode &n)
{
    if (proot != NULL)
    {
        delete proot;
        proot = NULL;
    }
    idHash.ClearAll();

    InitializeRoot(n);
    LLInitialize(n);
    while (!LLIsEmpty())
    {
        if (!LLGetFirst().IsSolved())
        {
            LLGetFirst().AddNextMoves();
        }
        //if (LLGetFirst().IsSolved())
        //  LLGetFirst().print(std::cout);
        LLSkipFirst();
    }
}

int KPnode::GetSolutionsCount(void)
{
    int i = 0;

    KPnode::LLSetFirstToRoot();
    while (!KPnode::LLIsEmpty())
    {
        if (KPnode::LLGetFirst().IsSolved())
        {
            i++;
        }
        KPnode::LLSkipFirst();
    }
    return i;
}

void KPnode::CalculateSolveCount(void)
{
    BTime time;

    time.ResetRelativeTime();
 
    try
    {
        KPnode *p;
        iterationCount = 0;
        solveCountAvailable = false;
        p = proot;
        while (p != NULL)
        {
            if (p->IsSolved())
            {
                // recursively modify the solve count
                p->RecursiveUpdateSolveCount(0);
            }
            p = p->pnext;
        }
    }
    catch (std::exception &e)
    {
        return;  // ignore exception during finalize
    };

    solveTime = time.GetRelativeTimeUsf(true);
    solveCountAvailable = true;
}

void KPnode::RecursiveUpdateSolveCount(short n)
{
    if ( finalizeInProgress )
    {
        throw std::logic_error("Process shutdown in progress.");
    }

    // Solutions with more than 120 moves are not of interest.
    // => Abort recursion.
    // This tremendously reduces the calculation time.
    if (n > 120 || (GetMovesToSolve() <= n))
    {
        return;
    }

    iterationCount++;
    SetMovesToSolve(n);

    std::vector<KPnode *>::iterator it;

    for (it = parents.begin(); it != parents.end(); ++it)
    {
        (*it)->RecursiveUpdateSolveCount(n + 1);
    }
}

void KPnode::PrintSolveCount(std::ostream &os)
{
    if (!IsSolveCountAvailable())
    {
        os << "KPnode: Solve count not yet available!" << std::endl;
        return;
    }
    else
    {
        KPnode::LLSetFirstToRoot();
        while (!KPnode::LLIsEmpty())
        {
            const KPnode *n = &LLGetFirst();
            os << ' ' << n->GetMovesToSolve() << std::endl;
            LLSkipFirst();
        }
    }
}

void KPnode::finalize(void)
{
    KPnode *p, *pn;
    finalizeInProgress = true;

#ifdef LINUX
    sleep(1);
#endif
#ifdef WIN32
    Sleep(200);  // 200 ms should be enough
#endif
    p = proot;
    proot = pfirst = plast = NULL;
    LLSize              = 0;
    iterationCount      = 0;
    solveCountAvailable = false;
    while (p != NULL)
    {
        pn = p->pnext;
        delete p;
        p = pn;
    };
}
