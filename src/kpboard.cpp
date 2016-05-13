/*
    kpboard.cpp


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

#include "stdafx.h"
#include <stdexcept>
#include <ostream>
#include <iomanip>
#include <algorithm>
#include "kpboard.h"


const KPboard::tArrayOfChar KPboard::xExtends
{
    1, 1, 1, 1, 1, 1, 1, 1, 2, 2
};
const KPboard::tArrayOfChar KPboard::yExtends
{
    1, 1, 1, 1, 2, 2, 2, 2, 1, 2
};

KPboard::KPboard() : id(0),
    positions{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    tokenID{ TK_EMPTY, TK_EMPTY, TK_EMPTY, TK_EMPTY, TK_EMPTY, TK_EMPTY,
             TK_EMPTY, TK_EMPTY, TK_EMPTY, TK_EMPTY, TK_EMPTY, TK_EMPTY,
             TK_EMPTY, TK_EMPTY, TK_EMPTY, TK_EMPTY, TK_EMPTY, TK_EMPTY,
             TK_EMPTY, TK_EMPTY }
{
}

KPboard::KPboard(const KPboard &src)
{
    id = 0;

    std::copy(src.positions.cbegin(), src.positions.cend(), positions.begin());

    memcpy(tokenID, src.tokenID, sizeof(tokenID));
}

KPboard &KPboard::operator=(const KPboard &src)
{
    id = 0;

    std::copy(src.positions.cbegin(), src.positions.cend(), positions.begin());

    memcpy(tokenID, src.tokenID, sizeof(tokenID));

    return *this;
}

bool KPboard::operator == (const KPboard &src) const
{
    return GetID() == src.GetID();
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

    for (auto i = 0; i < GetXExtend(aTokenId); i++)
    {
        for (auto j = 0; j < GetYExtend(aTokenId); j++)
        {
            tokenID[x + i][y + j] = aTokenId;
        }
    }

    positions[aTokenId].x = x;
    positions[aTokenId].y = y;
    id = 0;
}

void KPboard::InitializeToken(tKPTokenID aTokenId, int x, int y)
{
    SetPosition(aTokenId, x, y);
}

bool KPboard::Move(tKPTokenID aTokenId, tKPDirection direction)
{
    if (aTokenId == TK_EMPTY || direction == MOVE_NO)
    {
        return false;
    }

    auto x = positions[aTokenId].x;
    auto y = positions[aTokenId].y;
    auto xExtend = GetXExtend(aTokenId);
    auto yExtend = GetYExtend(aTokenId);
    decltype(xExtend) i;
    decltype(yExtend) j;

    switch (direction)
    {

        case MOVE_UP:

            // 1. check if move is possible and allowed
            if (y == 0)
            {
                return false;
            }

            for (i = 0; i < xExtend; i++)
            {
                if (tokenID[x + i][y - 1] != TK_EMPTY)
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
                tokenID[x + i][y + yExtend] = TK_EMPTY;
            }

            return true;

        case MOVE_DOWN:
            if (y + yExtend >= VERTICAL_MAX)
            {
                return false;
            }

            for (i = 0; i < xExtend; i++)
            {
                if (tokenID[x + i][y + yExtend] != TK_EMPTY)
                {
                    return false;
                }
            }

            y++;
            SetPosition(aTokenId, x, y);

            for (i = 0; i < xExtend; i++)
            {
                tokenID[x + i][y - 1] = TK_EMPTY;
            }

            return true;

        case MOVE_LEFT:
            if (x == 0)
            {
                return false;
            }

            for (j = 0; j < yExtend; j++)
            {
                if (tokenID[x - 1][y + j] != TK_EMPTY)
                {
                    return false;
                }
            }

            x--;
            SetPosition(aTokenId, x, y);

            for (j = 0; j < yExtend; j++)
            {
                tokenID[x + xExtend][y + j] = TK_EMPTY;
            }

            return true;

        case MOVE_RIGHT:
            if (x + xExtend >= HORIZONTAL_MAX)
            {
                return false;
            }

            for (j = 0; j < yExtend; j++)
            {
                if (tokenID[x + xExtend][y + j] != TK_EMPTY)
                {
                    return false;
                }
            }

            x++;
            SetPosition(aTokenId, x, y);

            for (j = 0; j < yExtend; j++)
            {
                tokenID[x - 1][y + j] = TK_EMPTY;
            }

            return true;

        case MOVE_NO:
            return false;  // satisfy compiler
    } // switch

    return false;  // the default
}

// Check if token aTokenId can move in direction d
// return true if it can move

bool KPboard::CanMove(tKPTokenID aTokenId, tKPDirection direction) const
{
    if (aTokenId == TK_EMPTY)
    {
        return false;
    }

    auto x = positions[aTokenId].x;
    auto y = positions[aTokenId].y;
    auto xExtend = GetXExtend(aTokenId);
    auto yExtend = GetYExtend(aTokenId);
    decltype(xExtend) i;

    switch (direction)
    {

        case MOVE_UP:
            if (y == 0)
            {
                return false;
            }

            for (i = 0; i < xExtend; i++)
            {
                if (tokenID[x + i][y - 1] != TK_EMPTY)
                {
                    return false;
                }
            }

            break;

        case MOVE_DOWN:
            if (y + yExtend >= VERTICAL_MAX)
            {
                return false;
            }

            for (i = 0; i < xExtend; i++)
            {
                if (tokenID[x + i][y + yExtend] != TK_EMPTY)
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
                if (tokenID[x - 1][y + i] != TK_EMPTY)
                {
                    return false;
                }
            }

            break;

        case MOVE_RIGHT:
            if (x + xExtend >= HORIZONTAL_MAX)
            {
                return false;
            }

            for (i = 0; i < yExtend; i++)
            {
                if (tokenID[x + xExtend][y + i] != TK_EMPTY)
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

std::vector<KPboard::KPmove> KPboard::GetPossibleMoves(void) const
{
    std::vector<KPboard::KPmove> moves;

    for (auto i = 0; i < HORIZONTAL_MAX; i++)
    {
        for (auto j = 0; j < VERTICAL_MAX; j++)
        {
            tKPTokenID token;

            if (tokenID[i][j] == TK_EMPTY)
            {
                // Try to move token above of TK_EMPTY
                if ((j > 0) &&
                    (token = static_cast<tKPTokenID>(tokenID[i][j - 1]),
                     CanMove(token, MOVE_DOWN)))
                {
                    KPboard::KPmove move(token, MOVE_DOWN);

                    if (std::find(moves.begin(), moves.end(), move) ==
                        moves.end())
                    {
                        moves.emplace_back(move);
                    }
                }

                // Try to move token below of TK_EMPTY
                if ((j < VERTICAL_MAX - 1) &&
                    (token = static_cast<tKPTokenID>(tokenID[i][j + 1]),
                     CanMove(token, MOVE_UP)))
                {
                    KPmove move(token, MOVE_UP);

                    if (std::find(moves.begin(), moves.end(), move) ==
                        moves.end())
                    {
                        moves.emplace_back(move);
                    }
                }

                // Try to move token left of TK_EMPTY
                if ((i > 0) &&
                    (token = static_cast<tKPTokenID>(tokenID[i - 1][j]),
                     CanMove(token, MOVE_RIGHT)))
                {
                    KPmove move(token, MOVE_RIGHT);

                    if (std::find(moves.begin(), moves.end(), move) ==
                        moves.end())
                    {
                        moves.emplace_back(move);
                    }
                }

                // Try to move token right of TK_EMPTY
                if ((i < HORIZONTAL_MAX - 1) &&
                    (token = static_cast<tKPTokenID>(tokenID[i + 1][j]),
                     CanMove(token, MOVE_LEFT)))
                {
                    KPmove move(token, MOVE_LEFT);

                    if (std::find(moves.begin(), moves.end(), move) ==
                        moves.end())
                    {
                        moves.emplace_back(move);
                    }
                }
            }
        }
    }

    return moves;
}

void KPboard::print(std::ostream &os) const
{
    for (auto j = 0; j < VERTICAL_MAX; j++)
    {
        for (auto i = 0; i < HORIZONTAL_MAX; i++)
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

    int index[TOKEN_MAX];
    bool assigned[TOKEN_MAX];

    for (auto x = 0; x < TOKEN_MAX; x++)
    {
        index[x]    = 0;
        assigned[x] = false;
    }

    auto greens = TK_GREEN1;
    auto whites_v = TK_WHITE1;

    for (auto y = 0; y < VERTICAL_MAX; y++)
    {
        for (auto x = 0; x < HORIZONTAL_MAX; x++)
        {
            if (tokenID[x][y] != TK_EMPTY && !assigned[tokenID[x][y]])
            {
                switch (tokenID[x][y])
                {
                    case TK_GREEN1:
                    case TK_GREEN2:
                    case TK_GREEN3:
                    case TK_GREEN4:
                        index[greens] = (x << 3) | y;
                        ++greens;
                        break;

                    case TK_WHITE1:
                    case TK_WHITE2:
                    case TK_WHITE3:
                    case TK_WHITE4:
                        index[whites_v] = (x << 3) | y;
                        ++whites_v;
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

KPboard KPboard::CreateRootBoard()
{
    KPboard rootBoard;

    // Create a board with all tokens in start position
    rootBoard.InitializeToken(TK_GREEN1, 1, 3);
    rootBoard.InitializeToken(TK_GREEN2, 2, 3);
    rootBoard.InitializeToken(TK_GREEN3, 1, 4);
    rootBoard.InitializeToken(TK_GREEN4, 2, 4);
    rootBoard.InitializeToken(TK_WHITE1, 0, 0);
    rootBoard.InitializeToken(TK_WHITE2, 3, 0);
    rootBoard.InitializeToken(TK_WHITE3, 0, 3);
    rootBoard.InitializeToken(TK_WHITE4, 3, 3);
    rootBoard.InitializeToken(TK_WHITE5, 1, 2);
    rootBoard.InitializeToken(TK_RED1,   1, 0);

    return rootBoard;
}

