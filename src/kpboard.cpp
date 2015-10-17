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
#include <ostream>
#include <iomanip>

KPIdHash KPboard::idHash;

KPboard::KPboard()
{
    Init();
}

KPboard::KPboard(const KPboard &b)
{
    CopyFrom(b);
}

bool KPboard::operator == (const KPboard &b) const
{
    return GetID() == b.GetID();
}

void KPboard::CopyFrom(const KPboard &src)
{
    id = 0;
    memcpy(token, src.token, sizeof(token));
    memcpy(tokenID, src.tokenID, sizeof(tokenID));
}


KPboard &KPboard::operator= (const KPboard &b)
{
    if (&b != this)
    {
        CopyFrom(b);
    }

    return *this;
}

void KPboard::Init(void)
{
    id = 0;
    for (int i = 0; i < TOKEN_MAX; ++i)
    {
        token[i] = 0;
    }
    memset(tokenID, TK_EMPTY, sizeof(tokenID));
}

KPboard::~KPboard()
{
}

inline void KPboard::SetPosition(tKPTokenID aTokenId, int x, int y, int xExtend,
                                 int yExtend)
{
    for (int i = 0; i < xExtend; i++)
    {
        for (int j = 0; j < yExtend; j++)
        {
            tokenID[x+i][y+j] = aTokenId;
        }
    }
}

bool KPboard::InitializeToken (tKPTokenID aTokenId, const char *, int x, int y,
                               int xExtend, int yExtend)
{
    if (aTokenId == TK_EMPTY)
    {
        return false;
    }
    token[aTokenId] = (xExtend << 12) | (yExtend << 8) | (x << 3) | y;
    SetPosition(aTokenId, x, y, xExtend, yExtend);
    id = 0;
    return true;
}

bool KPboard::Move(tKPTokenID aTokenID, tKPDirection d)
{
    if ( aTokenID == TK_EMPTY || d == MOVE_NO )
    {
        return false;
    }

    short unsigned int x, y, xExtend, yExtend;
    short unsigned int i, j;

    y       =  token[aTokenID]        & 0x7;
    x       = (token[aTokenID] >>  3) & 0xF;
    yExtend = (token[aTokenID] >>  8) & 0xF;
    xExtend = (token[aTokenID] >> 12) & 0xF;
    id = 0;

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
            SetPosition(aTokenID, x, y, xExtend, yExtend);

            // 4. set the new empty position
            for (i = 0; i < xExtend; i++)
            {
                tokenID[x+i][y+yExtend] = TK_EMPTY;
            }
            token[aTokenID] = (token[aTokenID] & ~0xFF) | (x << 3 | y);
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
            SetPosition(aTokenID, x, y, xExtend, yExtend);
            for (i = 0; i < xExtend; i++)
            {
                tokenID[x+i][y-1] = TK_EMPTY;
            }
            token[aTokenID] = (token[aTokenID] & ~0xFF) | (x << 3 | y);
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
            SetPosition(aTokenID, x, y, xExtend, yExtend);
            for (j = 0; j < yExtend; j++)
            {
                tokenID[x+xExtend][y+j] = TK_EMPTY;
            }
            token[aTokenID] = (token[aTokenID] & ~0xFF) | (x << 3 | y);
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
            SetPosition(aTokenID, x, y, xExtend, yExtend);
            for (j = 0; j < yExtend; j++)
            {
                tokenID[x-1][y+j] = TK_EMPTY;
            }
            token[aTokenID] = (token[aTokenID] & ~0xFF) | (x << 3 | y);
            return true;

        case MOVE_NO:
            return false;  // satisfy compiler
    } // switch

    return false;  // the default
}

// Check if token aTokenID can move in direction d
// return true if it can move

bool KPboard::CanMove(tKPTokenID aTokenID, tKPDirection d) const
{
    if (aTokenID == TK_EMPTY)
    {
        return false;
    }

    short unsigned int x, y, xExtend, yExtend;
    short unsigned int i;

    y       =  token[aTokenID]        & 0x7;
    x       = (token[aTokenID] >>  3) & 0xF;
    yExtend = (token[aTokenID] >>  8) & 0xF;
    xExtend = (token[aTokenID] >> 12) & 0xF;

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
            os << std::setw(2) << (int)tokenID[i][j] << ' ';
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
            switch (tokenID[x][y])
            {
                case TK_GREEN1:
                case TK_GREEN2:
                case TK_GREEN3:
                case TK_GREEN4:
                    if (!assigned[tokenID[x][y]])
                    {
                        index[greens++] = (x << 3) | y;
                        assigned[tokenID[x][y]] = true;
                    }
                    break;
                case TK_WHITE1:
                case TK_WHITE2:
                case TK_WHITE3:
                case TK_WHITE4:
                    if (!assigned[tokenID[x][y]])
                    {
                        index[whites_v++] = (x << 3) | y;
                        assigned[tokenID[x][y]] = true;
                    }
                    break;
                case TK_WHITE5:
                    if (!assigned[tokenID[x][y]])
                    {
                        index[TK_WHITE5] = (x << 3) | y;
                        assigned[tokenID[x][y]] = true;
                    }
                    break;
                case TK_RED1:
                    if (!assigned[tokenID[x][y]])
                    {
                        index[TK_RED1] = (x << 3) | y;
                        assigned[tokenID[x][y]] = true;
                    }
                    break;
            }
        }
    }

    tKPTokenID anID    = TK_GREEN1;
    do
    {
        id <<= 5;
        id |= index[anID] & 0x1F;
    }
    while (++anID != TK_GREEN1);

    return id;
}

bool KPboard::IsMemberOf()
{
    return KPboard::idHash.Contains(GetID());
}

bool KPboard::IsSolved(void) const
{
    return ((token[TK_RED1] & 0xFF) == ((1 << 3) | 3));
}

int KPboard::GetX(tKPTokenID aTokenID) const
{
    if (aTokenID == TK_EMPTY)
    {
        return 0;
    }

    return (token[aTokenID] >>  3) & 0xF;
}

int KPboard::GetY(tKPTokenID aTokenID) const
{
    if (aTokenID == TK_EMPTY)
    {
        return 0;
    }

    return token[aTokenID] & 0x7;
}

inline int KPboard::GetXExtend(tKPTokenID aTokenID) const
{
    if (aTokenID == TK_EMPTY)
    {
        return 0;
    }

    return (token[aTokenID] >> 12) & 0xF;
}

inline int KPboard::GetYExtend(tKPTokenID aTokenID) const
{
    if (aTokenID == TK_EMPTY)
    {
        return 0;
    }

    return (token[aTokenID] >>  8) & 0xF;
}
