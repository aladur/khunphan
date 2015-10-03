/*
    kpidhash.cpp


    Copyright (C) 2002-2015  W. Schwotzer

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

#include <iomanip>
#include "kpidhash.h"

#ifdef WIN32
#define CONST_0XFFF 0xFFFui64
#else
#define CONST_0XFFF 0xFFFL
#endif

KPIdHash::KPIdHash()
{
    hashTable = new KPIdHashEntry *[HASHTABLE_SIZE];
    for (int i = 0; i < HASHTABLE_SIZE; i++)
    {
        hashTable[i] = NULL;
    }
}

void KPIdHash::DeleteEntry(KPIdHashEntry *pEntry)
{
    // recursively delete the linear list of entries
    if (pEntry == NULL)
    {
        return;
    }
    if (pEntry->pnext)
    {
        DeleteEntry(pEntry->pnext);
    }
    delete pEntry;
}

void KPIdHash::ClearAll(void)
{
    for (int i = 0; i < HASHTABLE_SIZE; i++)
    {
        DeleteEntry(hashTable[i]);
        hashTable[i] = NULL;
    }
}

KPIdHash::~KPIdHash()
{
    ClearAll();
    delete[] hashTable;
    hashTable = NULL;
}

void KPIdHash::Add(const QWord &d, void *pObj /* = NULL */)
{
    KPIdHashEntry *p, *n, *newE;

    newE = new KPIdHashEntry;
    newE->data    = d;
    newE->pnext   = NULL;
    newE->pObject = pObj;
    tIdHash hash = GetHash(d);

    if ((p = hashTable[hash]) == NULL)
    {
        // add the first hash value to the beginning of the linked list
        hashTable[hash] = newE;
        return;
    }
    if (d < p->data)
    {
        // add a smaller hash value to the beginning of the linked list
        hashTable[hash] = newE;
        newE->pnext = p;
        return;
    }
    while ((n = p->pnext) != NULL)
    {
        if (d < n->data)
        {
            // add a new hash value in the midth of the linked list
            p->pnext = newE;
            newE->pnext = n;
            return;
        }
        // hash value already part of hash table: ignore it
        if (d == n->data)
        {
            delete newE;
            return;
        }
        p = p->pnext;
    }
    // add a larger hash value to the end of the linked list
    p->pnext = newE;
}

bool KPIdHash::Contains(const QWord &d) const
{
    KPIdHashEntry *e = hashTable[GetHash(d)];
    while (e != NULL)
    {
        if (d < e->data)
        {
            return false;
        }
        if (e->data == d)
        {
            return true;
        }
        e = e->pnext;
    }
    return false;
}

void *KPIdHash::GetObjectWith(const QWord &d) const
{
    KPIdHashEntry *e = hashTable[GetHash(d)];
    while (e != NULL)
    {
        if (d < e->data)
        {
            return NULL;
        }
        if (e->data == d)
        {
            return e->pObject;
        }
        e = e->pnext;
    }
    return NULL;
}

tIdHash KPIdHash::GetHash(QWord d) const
{
    return static_cast<tIdHash>
           (((d &  CONST_0XFFF) ^
             ((d & (CONST_0XFFF << 12)) >> 12) ^
             ((d & (CONST_0XFFF << 24)) >> 24) ^
             ((d & (CONST_0XFFF << 36)) >> 36)) % HASHTABLE_SIZE);
    /*
        return
            ((d & 0xFFFFLL) ^
            ((d & (0xFFFFLL << 16)) >> 16) ^
            ((d & (0xFFFFLL << 32)) >> 32)) % HASHTABLE_SIZE;
    */
}

void KPIdHash::print(std::ostream &os) const
{
    unsigned int i, sum;
    KPIdHashEntry *e;

    sum = 0;
    os << "KpIdHash:" << std::endl;
    for (i = 0; i < HASHTABLE_SIZE; i++)
    {
        unsigned int n = 0;
        e = hashTable[i];
        while (e != NULL)
        {
            n++;
            e = e->pnext;
        }
        sum += n;
        os << "  Hash " << i << ", "
           << std::setw(2) << n << " entries" << std::endl;
    }
    os << "  Total: " << sum << " entries" << std::endl;
}

void KPIdHash::Check(std::ostream &os) const
{
    int i;
    KPIdHashEntry *p, *n;

    os << "Checking KpIdHash" << std::endl;

    for (i = 0; i < HASHTABLE_SIZE; i++)
    {
        if ((p = hashTable[i]) == NULL)
        {
            continue;
        }
        while ((n = p->pnext) != NULL)
        {
            if (p->data == n->data)
            {
                os << "Hash id " << i << ": same data: "
                   << p->data << std::endl;
            }
            if (p->data > n->data)
            {
                os << "Hash id " << i << ": wrong order: " << p->data
                   << ", " << n->data << std::endl; 
            }
            p = p->pnext;
        }
    }
}
