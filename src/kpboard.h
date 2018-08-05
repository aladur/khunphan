/*
    kpboard.h


    Automatic solution finder for KhunPhan game
    Copyright (C) 2001-2018  W. Schwotzer

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


constexpr int HORIZONTAL_MAX = 4;
constexpr int VERTICAL_MAX = 5;

class KPboard
{
public:
    class KPmove
    {
    public:
        KPmove() : token(TokenId::EMPTY), direction(MoveToken::Not)
        {
        }
        KPmove(TokenId t, MoveToken d) : token(t), direction(d)
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
        inline TokenId GetToken() const
        {
            return token;
        }
        inline MoveToken GetDirection() const
        {
            return direction;
        }
    private:
        TokenId token;
        MoveToken direction;
    };

    typedef struct
    {
        std::uint8_t x;
        std::uint8_t y;
    } tPosition;

private:
    typedef std::array<tPosition, static_cast<std::size_t>(TokenId::COUNT)>
        tArrayOftPosition;
    typedef std::array<std::uint8_t, static_cast<std::size_t>(TokenId::COUNT)>
        tArrayOfUInt8;

    mutable QWord id;
    tArrayOftPosition positions;
    TokenId tokenID[HORIZONTAL_MAX][VERTICAL_MAX];
    static const tArrayOfUInt8 xExtends;
    static const tArrayOfUInt8 yExtends;

public:

    KPboard();
    KPboard(const KPboard &b);
    KPboard & operator=(const KPboard &b);

    bool operator == (const KPboard &b) const;
    void print(std::ostream &os) const;
    void InitializeToken(TokenId id, int x, int y);
    inline bool Move(const KPmove &move)
    {
        return Move(move.GetToken(), move.GetDirection());
    }
    bool Move(TokenId id, MoveToken d);
    bool CanMove(TokenId aTokenId, MoveToken d) const;
    std::vector<KPboard::KPmove> GetPossibleMoves(void) const;
    QWord GetID() const;
    inline bool IsSolved(void) const
    {
        return (positions[static_cast<std::size_t>(TokenId::RED1)].x == 1 &&
                positions[static_cast<std::size_t>(TokenId::RED1)].y == 3);
    }
    inline int GetX(TokenId aTokenId) const
    {
        if (aTokenId == TokenId::EMPTY)
        {
            return 0;
        } else
        {
            return positions[static_cast<std::size_t>(aTokenId)].x;
        }
    }
    inline int GetY(TokenId aTokenId) const
    {
        if (aTokenId == TokenId::EMPTY)
        {
            return 0;
        } else
        {
            return positions[static_cast<std::size_t>(aTokenId)].y;
        }
    }
    static inline int GetXExtend(TokenId aTokenId)
    {
        if (aTokenId == TokenId::EMPTY)
        {
            return 1;
        } else
        {
            return xExtends[static_cast<std::size_t>(aTokenId)];
        }
    }
    static inline int GetYExtend(TokenId aTokenId)
    {
        if (aTokenId == TokenId::EMPTY)
        {
            return 1;
        } else
        {
            return yExtends[static_cast<std::size_t>(aTokenId)];
        }
    }
    static KPboard CreateRootBoard();

private:
    inline void SetPosition(TokenId id, int x, int y);
};

#endif
