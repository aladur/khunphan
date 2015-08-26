/*
    kpidhash.h


    Copyright (C) 2001, 2002  W. Schwotzer

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

#ifndef __kpidhash_h__
#define __kpidhash_h__

#include "misc1.h"

#define HASHTABLE_SIZE  4097


class KPnode;

typedef unsigned short tIdHash;

class KPIdHash {
	friend class KPnode;
public:
  struct KPIdHashEntry {
    uint64_t	data;
    KPIdHashEntry *pnext;
    void *pObject;
  };
private:
	tIdHash	GetHash(uint64_t d) const;
	KPIdHashEntry **hashTable;
	void DeleteEntry(KPIdHashEntry *pEntry);	
protected:
	void *GetObjectWith(const uint64_t &d) const;
public:
	KPIdHash();
  virtual ~KPIdHash();
	void Add(const uint64_t &d, void *pObj = NULL);
	bool Contains(const uint64_t &d) const;
	void ClearAll(void);
	void fprintf(FILE *fp) const;
	void Check(FILE *fp) const;
};

#endif  // __ kpidhash_h__
