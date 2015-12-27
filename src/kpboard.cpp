/*
    kpboard.cpp


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

#include "misc1.h"  // needed for memset and NULL
#include "kpboard.h"
#include <stdexcept>
#include <ostream>
#include <iomanip>


const char KPboard::xExtend[TOKEN_MAX] = { 1, 1, 1, 1, 1, 1, 1, 1, 2, 2 };
const char KPboard::yExtend[TOKEN_MAX] = { 1, 1, 1, 1, 2, 2, 2, 2, 1, 2 };

KPboard::KPboard()
{
    id = 0;
    for (int i = 0; i < TOKEN_MAX; ++i)
    {
        position[i].x = 0;
        position[i].y = 0;
    }
    memset(tokenID, TK_EMPTY, sizeof(tokenID));
}

KPboard::KPboard(const KPboard &src)
{
    id = 0;
    for (int i = 0; i < TOKEN_MAX; ++i)
    {
        position[i].x = src.position[i].x;
        position[i].y = src.position[i].y;
    }
    memcpy(tokenID, src.tokenID, sizeof(tokenID));
}

bool KPboard::operator == (const KPboard &b) const
{
    return GetID() == b.GetID();
}

inline void KPboard::SetPosition(tKPTokenID aTokenId, int x, int y)
{
    if (aTokenId == TK_EMPTY)
    {
        throw std::invalid_argument("TK_EMPTY can not be initialized");
    }

    if (x < 0 || y < 0 ||
        x + GetXExtend(aTokenId) > HORIZONTAL_MAX ||
        y + GetYExtend(aTokenId) > VERTICAL_MAX)
    {
        throw std::invalid_argument("x or y out of range");
    }

    for (int i = 0; i < GetXExtend(aTokenId); i++)
    {
        for (int j = 0; j < GetYExtend(aTokenId); j++)
        {
            tokenID[x+i][y+j] = aTokenId;
        }
    }

    position[aTokenId].x = x;
    position[aTokenId].y = y;
    id = 0;
}

void KPboard::InitializeToken(tKPTokenID aTokenId, int x, int y)
{
    SetPosition(aTokenId, x, y);
}

bool KPboard::Move(tKPTokenID aTokenId, tKPDirection d)
{
    if ( aTokenId == TK_EMPTY || d == MOVE_NO )
    {
        return false;
    }

    short unsigned int x, y, xExtend, yExtend;
    short unsigned int i, j;

    x = position[aTokenId].x;
    y = position[aTokenId].y;
    xExtend = GetXExtend(aTokenId);
    yExtend = GetYExtend(aTokenId);

    switch (d)
    {

        case MOVE_UP:
            // 1. check if move is possible and allowed
            if (y == 0)
            {
                return false;
            }
            for (i = 0; i < xExtend; i++)
            {
                if (tokenID[x+i][y-1] != TK_EMPTY)
                {
                    return false;
                }
            }

            // 2. update the position of this token
            y--;

            // 3. Update the position in the board
            SetPosition(aTokenId, x, y);

            // 4. set the new empty position
            for (i = 0; i < xExtend; i++)
            {
                tokenID[x+i][y+yExtend] = TK_EMPTY;
            }
            return true;

        case MOVE_DOWN:
            if (y+yExtend >= VERTICAL_MAX)
            {
                return false;
            }
            for (i = 0; i < xExtend; i++)
            {
                if (tokenID[x+i][y+yExtend] != TK_EMPTY)
                {
                    return false;
                }
            }
            y++;
            SetPosition(aTokenId, x, y);
            for (i = 0; i < xExtend; i++)
            {
                tokenID[x+i][y-1] = TK_EMPTY;
            }
            return true;

        case MOVE_LEFT:
            if (x == 0)
            {
                return false;
            }
            for (j = 0; j < yExtend; j++)
            {
                if (tokenID[x-1][y+j] != TK_EMPTY)
                {
                    return false;
                }
            }
            x--;
            SetPosition(aTokenId, x, y);
            for (j = 0; j < yExtend; j++)
            {
                tokenID[x+xExtend][y+j] = TK_EMPTY;
            }
            return true;

        case MOVE_RIGHT:
            if (x+xExtend >= HORIZONTAL_MAX)
            {
                return false;
            }
            for (j = 0; j < yExtend; j++)
            {
                if (tokenID[x+xExtend][y+j] != TK_EMPTY)
                {
                    return false;
                }
            }
            x++;
            SetPosition(aTokenId, x, y);
            for (j = 0; j < yExtend; j++)
            {
                tokenID[x-1][y+j] = TK_EMPTY;
            }
            return true;

        case MOVE_NO:
            return false;  // satisfy compiler
    } // switch

    return false;  // the default
}

// Check if token aTokenId can move in direction d
// return true if it can move

bool KPboard::CanMove(tKPTokenID aTokenId, tKPDirection d) const
{
    if (aTokenId == TK_EMPTY)
    {
        return false;
    }

    short unsigned int x, y, xExtend, yExtend;
    short unsigned int i;

    x = position[aTokenId].x;
    y = position[aTokenId].y;
    xExtend = GetXExtend(aTokenId);
    yExtend = GetYExtend(aTokenId);

    switch (d)
    {

        case MOVE_UP:
            if (y == 0)
            {
                return false;
            }
            for (i = 0; i < xExtend; i++)
            {
                if (tokenID[x+i][y-1] != TK_EMPTY)
                {
                    return false;
                }
            }
            break;

        case MOVE_DOWN:
            if (y+yExtend >= VERTICAL_MAX)
            {
                return false;
            }
            for (i = 0; i < xExtend; i++)
            {
                if (tokenID[x+i][y+yExtend] != TK_EMPTY)
                {
                    return false;
                }
            }
            break;

        case MOVE_LEFT:
            if (x == 0)
            {
                return false;
            }
            for (i = 0; i < yExtend; i++)
            {
                if (tokenID[x-1][y+i] != TK_EMPTY)
                {
                    return false;
                }
            }
            break;

        case MOVE_RIGHT:
            if (x+xExtend >= HORIZONTAL_MAX)
            {
                return false;
            }
            for (i = 0; i < yExtend; i++)
            {
                if (tokenID[x+xExtend][y+i] != TK_EMPTY)
                {
                    return false;
                }
            }
            break;

        case MOVE_NO:
            break;
    } // switch

    return true;  // the default
}

void KPboard::print(std::ostream &os) const
{
    for (int j = 0; j < VERTICAL_MAX; j++)
    {
        for (int i = 0; i < HORIZONTAL_MAX; i++)
        {
            os << std::setw(2) << static_cast<int>(tokenID[i][j]) << ' ';
        }
        os << std::endl;
    }
     os << "ID: " << GetID() << std::endl;
}

QWord KPboard::GetID() const
{
    // internal caching: if id is zero it first has to be calculated

    if (id)
    {
        return id;
    }

    unsigned short x, y;
    unsigned short greens, whites_v;
    unsigned short index[TOKEN_MAX];
    bool assigned[TOKEN_MAX];

    for (x = 0; x < TOKEN_MAX; x++)
    {
        index[x]    = 0;
        assigned[x] = false;
    }

    greens   = TK_GREEN1;
    whites_v = TK_WHITE1;

    for (y = 0; y < VERTICAL_MAX; y++)
    {
        for (x = 0; x < HORIZONTAL_MAX; x++)
        {
            if (tokenID[x][y] != TK_EMPTY && !assigned[tokenID[x][y]])
            {
                switch (tokenID[x][y])
                {
                    case TK_GREEN1:
                    case TK_GREEN2:
                    case TK_GREEN3:
                    case TK_GREEN4:
                        index[greens++] = (x << 3) | y;
                        break;
                    case TK_WHITE1:
                    case TK_WHITE2:
                    case TK_WHITE3:
                    case TK_WHITE4:
                        index[whites_v++] = (x << 3) | y;
                        break;
                    case TK_WHITE5:
                        index[TK_WHITE5] = (x << 3) | y;
                        break;
                    case TK_RED1:
                        index[TK_RED1] = (x << 3) | y;
                        break;
                }
                assigned[tokenID[x][y]] = true;
            }
        }
    }

    tKPTokenID anID = TK_GREEN1;
    do
    {
        id <<= 5;
        id |= index[anID] & 0x1F;
    }
    while (++anID != TK_GREEN1);

    return id;
}

