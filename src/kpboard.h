/*
    kpboard.h


    Automatic solution finder for KhunPhan game
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

#ifndef KPBOARD_H
#define KPBOARD_H

#include "stdafx.h"
#include <ostream>
#include <vector>
#include <array>


enum
{
    HORIZONTAL_MAX = 4,
    VERTICAL_MAX   = 5
};

class KPboard
{
public:
    class KPmove
    {
    public:
        KPmove() : token(TK_EMPTY), direction(MOVE_NO)
        {
        }
        KPmove(tKPTokenID t, tKPDirection d) : token(t), direction(d)
        {
        }
        KPmove(const KPmove &src)
        {
            token = src.token;
            direction = src.direction;
        }
        KPmove &operator=(const KPmove &src)
        {
            token = src.token;
            direction = src.direction;
            return *this;
        }
        bool operator==(const KPmove &rhs)
        {
            return token == rhs.token && (direction == rhs.direction);
        }
        inline tKPTokenID GetToken() const
        {
            return token;
        }
        inline tKPDirection GetDirection() const
        {
            return direction;
        }
    private:
        tKPTokenID token;
        tKPDirection direction;
    };

    typedef struct
    {
        unsigned char x;
        unsigned char y;
    } tPosition;

private:
    enum
    {
        TOKEN_MAX = 10
    };
    typedef std::array<tPosition, TOKEN_MAX> tArrayOftPosition;
    typedef std::array<char, TOKEN_MAX> tArrayOfChar;

    mutable QWord id;
    tArrayOftPosition positions;
    unsigned char tokenID[HORIZONTAL_MAX][VERTICAL_MAX];
    static const tArrayOfChar xExtends;
    static const tArrayOfChar yExtends;

public:

    KPboard();
    KPboard(const KPboard &b);

    bool operator == (const KPboard &b) const;
    void print(std::ostream &os) const;
    void InitializeToken(tKPTokenID id, int x, int y);
    inline bool Move(const KPmove &move)
    {
        return Move(move.GetToken(), move.GetDirection());
    }
    bool Move(tKPTokenID id, tKPDirection d);
    bool CanMove(tKPTokenID aTokenId, tKPDirection d) const;
    std::vector<KPboard::KPmove> GetPossibleMoves(void) const;
    QWord GetID() const;
    inline bool IsSolved(void) const
    {
        return (positions[TK_RED1].x == 1 && positions[TK_RED1].y == 3);
    }
    inline int GetX(tKPTokenID aTokenId) const
    {
        return (aTokenId == TK_EMPTY) ? 0 : positions[aTokenId].x;
    }
    inline int GetY(tKPTokenID aTokenId) const
    {
        return (aTokenId == TK_EMPTY) ? 0 : positions[aTokenId].y;
    }
    static inline int GetXExtend(tKPTokenID aTokenId)
    {
        return (aTokenId == TK_EMPTY) ? 1 : xExtends[aTokenId];
    }
    static inline int GetYExtend(tKPTokenID aTokenId)
    {
        return (aTokenId == TK_EMPTY) ? 1 : yExtends[aTokenId];
    }
    static KPboard CreateRootBoard();

private:
    inline void SetPosition(tKPTokenID id, int x, int y);
};

#endif
