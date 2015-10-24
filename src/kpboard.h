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

#include <ostream>
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
    typedef struct
    {
        unsigned char x;
        unsigned char y;
    } tPosition;

private:
    mutable QWord id;
    tPosition position[TOKEN_MAX];
    unsigned char tokenID[HORIZONTAL_MAX][VERTICAL_MAX];
    static const char xExtend[TOKEN_MAX];
    static const char yExtend[TOKEN_MAX];

public:

    KPboard();
    KPboard(const KPboard &b);

    bool operator == (const KPboard &b) const;
    void print(std::ostream &os) const;
    void InitializeToken (tKPTokenID id, int x, int y);
    inline void SetPosition(tKPTokenID id, int x, int y);
    bool Move(tKPTokenID id, tKPDirection d);
    bool CanMove(tKPTokenID aTokenId, tKPDirection d) const;
    QWord GetID() const;
    static KPIdHash idHash;
    inline bool IsMemberOf(void)
    {
        return KPboard::idHash.Contains(GetID());
    }
    inline bool IsSolved(void) const
    {
        return (position[TK_RED1].x == 1 && position[TK_RED1].y == 3);
    }
    inline int GetX(tKPTokenID aTokenId) const
    {
        return (aTokenId == TK_EMPTY) ? 0 : position[aTokenId].x;
    }
    inline int GetY(tKPTokenID aTokenId) const
    {
        return (aTokenId == TK_EMPTY) ? 0 : position[aTokenId].y;
    }
    static inline int GetXExtend(tKPTokenID aTokenId)
    {
        return (aTokenId == TK_EMPTY) ? 1 : xExtend[aTokenId];
    }
    static inline int GetYExtend(tKPTokenID aTokenId)
    {
        return (aTokenId == TK_EMPTY) ? 1 : yExtend[aTokenId];
    }
};

#endif
