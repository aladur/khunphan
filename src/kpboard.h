/*
    kpboard.h


    Automatic solution finder for KhunPhan game
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

#ifndef __kpboard_h__
#define __kpboard_h__

#include <stdio.h>
#include "misc1.h"
#include "kpidhash.h"


enum
{
    HORIZONTAL_MAX = 4,
    VERTICAL_MAX   = 5,
    MOVES_MAX      = 7,
    TOKEN_MAX      = 10
};

class KPboard
{
private:
    mutable QWord id;
    unsigned short token[TOKEN_MAX];
    unsigned char  tokenID[HORIZONTAL_MAX][VERTICAL_MAX];

public:

    KPboard();
    KPboard(const KPboard &b);
    virtual ~KPboard();

    bool operator == (const KPboard &b) const;
    KPboard &operator = (const KPboard &b);
    virtual void Init();
    bool IsMemberOf(void);
    virtual void fprintf(FILE *fp) const;
    bool InitializeToken (tKPTokenID id, const char *color, int x, int y,
                          int xExtend, int yExtend);
    inline void SetPosition(tKPTokenID id, int x, int y, int xExtend,
                            int yExtend);
    bool Move(tKPTokenID id, tKPDirection d);
    bool CanMove(tKPTokenID aTokenID, tKPDirection d) const;
    virtual void CopyFrom(const KPboard &src);
    QWord GetID() const;
    static KPIdHash idHash;
    bool IsSolved(void) const;
    int GetX(tKPTokenID aTokenID) const;
    int GetY(tKPTokenID aTokenID) const;
    inline int GetXExtend(tKPTokenID aTokenID) const;
    inline int GetYExtend(tKPTokenID aTokenID) const;
};

#endif
