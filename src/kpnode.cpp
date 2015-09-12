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
#ifdef HAVE_STDLIB_H
/* on some systems stdio.h is a prerequisite to stdlib.h */
#include <stdio.h>
#include <stdlib.h>
#endif
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <limits.h>
#include "kpnode.h"


KPnode *KPnode::proot  = NULL;
KPnode *KPnode::pfirst = NULL;
KPnode *KPnode::plast  = NULL;
unsigned int KPnode::LLSize = 0;
bool KPnode::solveCountAvailable = false;
int  KPnode::iterationCount = 0;
bool KPnode::finalizeInProgress = false;

KPnode::KPnode() : pnext(NULL), movesToSolve(SHRT_MAX)
{
    memset(pchild,  0, sizeof(pchild));
    memset(pparent, 0, sizeof(pparent));
}

KPnode::KPnode(const KPnode &src) : KPboard (src), pnext(NULL),
    movesToSolve(SHRT_MAX)
{
    KPboard::CopyFrom(src);
    movesToSolve = src.GetMovesToSolve();
    for (int i = 0; i < MOVES_MAX; i++)
    {
        ( src.pchild[i]  != NULL ) ? pchild[i]  = new KPnode(*src.pchild[i]) :
                                     pchild[i]  = NULL;
        ( src.pparent[i] != NULL ) ? pparent[i] = new KPnode(*src.pparent[i]) :
                                     pparent[i] = NULL;
    }
}

KPnode::KPnode(const KPboard &src) : KPboard (src), pnext(NULL),
    movesToSolve(SHRT_MAX)
{
    KPboard::CopyFrom(src);
    memset(pchild,  0, sizeof(pchild));
    memset(pparent, 0, sizeof(pparent));
}

void KPnode::CopyFrom(const KPnode &src)
{
    KPboard::CopyFrom(src);
    // ATTENTION: pparent and pnext not assigned!
    for (int i = 0; i < MOVES_MAX; i++)
    {
        if (pchild[i]  != NULL)
        {
            delete pchild[i];
        }
        if (pparent[i] != NULL)
        {
            delete pparent[i];
        }
        ( src.pchild[i]  != NULL ) ? pchild[i]  = new KPnode(*src.pchild[i]) :
                                     pchild[i]  = NULL;
        ( src.pparent[i] != NULL ) ? pparent[i] = new KPnode(*src.pparent[i]) :
                                     pparent[i] = NULL;
    }
}

KPnode &KPnode::operator= (const KPnode &b)
{
    CopyFrom(b);
    return *this;
}

KPnode::~KPnode()
{
}

int KPnode::GetNextFreeParentIdx(void) const
{
    int i = 0;
    while (i < MOVES_MAX)
    {
        if ( pparent[i] == NULL)
        {
            return i;
        }
        i++;
    }
    // Should not happen:
    DEBUGPRINT1("KPnode::GetNextFreeParentIdx: Too much parents in one node: "
                "%d\n",
                i);
    return 0;
}

inline bool KPnode::AddNextMoves()
{
    KPnode *pn;
    static KPboard b;
    static unsigned int no = 0;
    unsigned short i = 0;
    tKPTokenID id = TK_GREEN1;

    do
    {
        tKPDirection direction;

        direction = MOVE_UP;
        do
        {
            b.CopyFrom(*this);
            if (b.Move(id, direction))
            {
                if (!b.IsMemberOf())
                {
                    if (i >= MOVES_MAX)
                    {
                        DEBUGPRINT1("KPnode::AddNextMoves: Too much moves in "
                                    "one node: %u\n", i);
                    }
                    pn = new KPnode(b);
                    pchild[i++] = pn;
                    pn->pparent[pn->GetNextFreeParentIdx()] = this;
                    ++no;
                    KPnode::LLAddLast(*pn);
                    KPboard::idHash.Add(pn->GetID(), pn);
#ifdef DEBUG_OUTPUT
                    ::fprintf(stdout, "%u. New:\n", no);
                    pn->fprintf(stdout);
#endif
                }
                else
                {
                    // position already in tree
                    // Create parent and child links to existing node
                    pn = GetNodeFor(b);
                    pchild[i++] = pn;
                    pn->pparent[pn->GetNextFreeParentIdx()] = this;
                }
            } // if
        }
        while (++direction != MOVE_UP);
    }
    while (++id != TK_GREEN1);
    return true;
}

void KPnode::fprintfr(FILE *fp) const
{
    unsigned short i;

    ::fprintf(fp, "Node:\n");
    this->fprintf(fp);
    for (i = 0; i < MOVES_MAX; i++)
    {
        if (pchild[i] != NULL)
        {
            ::fprintf(fp, "%d. Child:\n", i+1);
            pchild[i]->fprintf(fp);
        }
    }
    ::fprintf(fp, "\n");
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
        //  LLGetFirst().fprintfr(stdout);
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

void KPnode::SetSolveCount(void)
{
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
                p->ModifySolveCount(0);
            }
            p = p->pnext;
        }
    }
    catch (ex &/* e */)
    {
        return;  // ignore exception during finalize
    };

    solveCountAvailable = true;
}

void KPnode::ModifySolveCount(short n) const
{
    if ( finalizeInProgress )
    {
        throw e;
    }

    iterationCount++;
    SetMovesToSolve(n);
    //for (i = 0; i < MOVES_MAX; i++) {
    //  if ( pparent[i] != NULL && (pparent[i]->GetMovesToSolve() > (n + 1)) )
    //    pparent[i]->ModifySolveCount(n + 1);
    //}
    // Loop expanded for performance reasons
    // Attention: Repetition count depends on value MOVES_MAX!
    short n_p_1 = n + 1;
    if ( pparent[0] != NULL && (pparent[0]->GetMovesToSolve() > n_p_1))
    {
        pparent[0]->ModifySolveCount(n_p_1);
    }
    if ( pparent[1] != NULL && (pparent[1]->GetMovesToSolve() > n_p_1))
    {
        pparent[1]->ModifySolveCount(n_p_1);
    }
    if ( pparent[2] != NULL && (pparent[2]->GetMovesToSolve() > n_p_1))
    {
        pparent[2]->ModifySolveCount(n_p_1);
    }
    if ( pparent[3] != NULL && (pparent[3]->GetMovesToSolve() > n_p_1))
    {
        pparent[3]->ModifySolveCount(n_p_1);
    }
    if ( pparent[4] != NULL && (pparent[4]->GetMovesToSolve() > n_p_1))
    {
        pparent[4]->ModifySolveCount(n_p_1);
    }
    if ( pparent[5] != NULL && (pparent[5]->GetMovesToSolve() > n_p_1))
    {
        pparent[5]->ModifySolveCount(n_p_1);
    }
    if ( pparent[6] != NULL && (pparent[6]->GetMovesToSolve() > n_p_1))
    {
        pparent[6]->ModifySolveCount(n_p_1);
    }
}

void KPnode::PrintSolveCount(FILE *fp)
{

    if (!IsSolveCountAvailable())
    {
        ::fprintf(fp, "KPnode: Solve count not yet available!\n");
        return;
    }
    else
    {
        KPnode::LLSetFirstToRoot();
        while (!KPnode::LLIsEmpty())
        {
            const KPnode *n = &LLGetFirst();
            ::fprintf(fp, " %d\n", n->GetMovesToSolve());
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
