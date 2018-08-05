/*
    kpboard.cpp


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

#include "stdafx.h"
#include <stdexcept>
#include <ostream>
#include <iomanip>
#include <algorithm>
#include "kpboard.h"


const KPboard::tArrayOfUInt8 KPboard::xExtends
{
    1, 1, 1, 1, 1, 1, 1, 1, 2, 2
};
const KPboard::tArrayOfUInt8 KPboard::yExtends
{
    1, 1, 1, 1, 2, 2, 2, 2, 1, 2
};

KPboard::KPboard() : id(0),
    positions{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    tokenID{ TokenId::EMPTY, TokenId::EMPTY, TokenId::EMPTY, TokenId::EMPTY,
             TokenId::EMPTY, TokenId::EMPTY, TokenId::EMPTY, TokenId::EMPTY,
             TokenId::EMPTY, TokenId::EMPTY, TokenId::EMPTY, TokenId::EMPTY,
             TokenId::EMPTY, TokenId::EMPTY, TokenId::EMPTY, TokenId::EMPTY,
             TokenId::EMPTY, TokenId::EMPTY, TokenId::EMPTY, TokenId::EMPTY
           }
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

inline void KPboard::SetPosition(TokenId aTokenId, int x, int y)
{
    if (aTokenId == TokenId::EMPTY)
    {
        throw std::invalid_argument("TokenId::EMPTY can not be initialized");
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

    positions[static_cast<std::size_t>(aTokenId)].x = x;
    positions[static_cast<std::size_t>(aTokenId)].y = y;
    id = 0;
}

void KPboard::InitializeToken(TokenId aTokenId, int x, int y)
{
    SetPosition(aTokenId, x, y);
}

bool KPboard::Move(TokenId aTokenId, MoveToken direction)
{
    if (aTokenId == TokenId::EMPTY || direction == MoveToken::Not)
    {
        return false;
    }

    auto x = positions[static_cast<std::size_t>(aTokenId)].x;
    auto y = positions[static_cast<std::size_t>(aTokenId)].y;
    auto xExtend = GetXExtend(aTokenId);
    auto yExtend = GetYExtend(aTokenId);
    decltype(xExtend) i;
    decltype(yExtend) j;

    switch (direction)
    {

        case MoveToken::Up:

            // 1. check if move is possible and allowed
            if (y == 0)
            {
                return false;
            }

            for (i = 0; i < xExtend; i++)
            {
                if (tokenID[x + i][y - 1] != TokenId::EMPTY)
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
                tokenID[x + i][y + yExtend] = TokenId::EMPTY;
            }

            return true;

        case MoveToken::Down:
            if (y + yExtend >= VERTICAL_MAX)
            {
                return false;
            }

            for (i = 0; i < xExtend; i++)
            {
                if (tokenID[x + i][y + yExtend] != TokenId::EMPTY)
                {
                    return false;
                }
            }

            y++;
            SetPosition(aTokenId, x, y);

            for (i = 0; i < xExtend; i++)
            {
                tokenID[x + i][y - 1] = TokenId::EMPTY;
            }

            return true;

        case MoveToken::Left:
            if (x == 0)
            {
                return false;
            }

            for (j = 0; j < yExtend; j++)
            {
                if (tokenID[x - 1][y + j] != TokenId::EMPTY)
                {
                    return false;
                }
            }

            x--;
            SetPosition(aTokenId, x, y);

            for (j = 0; j < yExtend; j++)
            {
                tokenID[x + xExtend][y + j] = TokenId::EMPTY;
            }

            return true;

        case MoveToken::Right:
            if (x + xExtend >= HORIZONTAL_MAX)
            {
                return false;
            }

            for (j = 0; j < yExtend; j++)
            {
                if (tokenID[x + xExtend][y + j] != TokenId::EMPTY)
                {
                    return false;
                }
            }

            x++;
            SetPosition(aTokenId, x, y);

            for (j = 0; j < yExtend; j++)
            {
                tokenID[x - 1][y + j] = TokenId::EMPTY;
            }

            return true;

        case MoveToken::Not:
            return false;  // satisfy compiler
    } // switch

    return false;  // the default
}

// Check if token aTokenId can move in direction d
// return true if it can move

bool KPboard::CanMove(TokenId aTokenId, MoveToken direction) const
{
    if (aTokenId == TokenId::EMPTY)
    {
        return false;
    }

    auto x = positions[static_cast<std::size_t>(aTokenId)].x;
    auto y = positions[static_cast<std::size_t>(aTokenId)].y;
    auto xExtend = GetXExtend(aTokenId);
    auto yExtend = GetYExtend(aTokenId);
    decltype(xExtend) i;

    switch (direction)
    {

        case MoveToken::Up:
            if (y == 0)
            {
                return false;
            }

            for (i = 0; i < xExtend; i++)
            {
                if (tokenID[x + i][y - 1] != TokenId::EMPTY)
                {
                    return false;
                }
            }

            break;

        case MoveToken::Down:
            if (y + yExtend >= VERTICAL_MAX)
            {
                return false;
            }

            for (i = 0; i < xExtend; i++)
            {
                if (tokenID[x + i][y + yExtend] != TokenId::EMPTY)
                {
                    return false;
                }
            }

            break;

        case MoveToken::Left:
            if (x == 0)
            {
                return false;
            }

            for (i = 0; i < yExtend; i++)
            {
                if (tokenID[x - 1][y + i] != TokenId::EMPTY)
                {
                    return false;
                }
            }

            break;

        case MoveToken::Right:
            if (x + xExtend >= HORIZONTAL_MAX)
            {
                return false;
            }

            for (i = 0; i < yExtend; i++)
            {
                if (tokenID[x + xExtend][y + i] != TokenId::EMPTY)
                {
                    return false;
                }
            }

            break;

        case MoveToken::Not:
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
            TokenId token;

            if (tokenID[i][j] == TokenId::EMPTY)
            {
                // Try to move token above of TokenId::EMPTY
                if ((j > 0) &&
                    (token = static_cast<TokenId>(tokenID[i][j - 1]),
                     CanMove(token, MoveToken::Down)))
                {
                    KPboard::KPmove move(token, MoveToken::Down);

                    if (std::find(moves.begin(), moves.end(), move) ==
                        moves.end())
                    {
                        moves.emplace_back(move);
                    }
                }

                // Try to move token below of TokenId::EMPTY
                if ((j < VERTICAL_MAX - 1) &&
                    (token = static_cast<TokenId>(tokenID[i][j + 1]),
                     CanMove(token, MoveToken::Up)))
                {
                    KPmove move(token, MoveToken::Up);

                    if (std::find(moves.begin(), moves.end(), move) ==
                        moves.end())
                    {
                        moves.emplace_back(move);
                    }
                }

                // Try to move token left of TokenId::EMPTY
                if ((i > 0) &&
                    (token = static_cast<TokenId>(tokenID[i - 1][j]),
                     CanMove(token, MoveToken::Right)))
                {
                    KPmove move(token, MoveToken::Right);

                    if (std::find(moves.begin(), moves.end(), move) ==
                        moves.end())
                    {
                        moves.emplace_back(move);
                    }
                }

                // Try to move token right of TokenId::EMPTY
                if ((i < HORIZONTAL_MAX - 1) &&
                    (token = static_cast<TokenId>(tokenID[i + 1][j]),
                     CanMove(token, MoveToken::Left)))
                {
                    KPmove move(token, MoveToken::Left);

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

    const int tokenCount = static_cast<std::size_t>(TokenId::COUNT);
    int index[tokenCount];
    bool assigned[tokenCount];
    std::size_t tokenIndex;

    for (auto x = 0; x < tokenCount; x++)
    {
        index[x] = 0;
        assigned[x] = false;
    }

    auto greensIndex = static_cast<std::size_t>(TokenId::GREEN1);
    auto whitesVerticalIndex = static_cast<std::size_t>(TokenId::WHITE1);

    for (auto y = 0; y < VERTICAL_MAX; y++)
    {
        for (auto x = 0; x < HORIZONTAL_MAX; x++)
        {
            tokenIndex = static_cast<std::size_t>(tokenID[x][y]);

            if (tokenID[x][y] != TokenId::EMPTY && !assigned[tokenIndex])
            {
                switch (tokenID[x][y])
                {
                    case TokenId::GREEN1:
                    case TokenId::GREEN2:
                    case TokenId::GREEN3:
                    case TokenId::GREEN4:
                        index[greensIndex] = (x << 3) | y;
                        ++greensIndex;
                        break;

                    case TokenId::WHITE1:
                    case TokenId::WHITE2:
                    case TokenId::WHITE3:
                    case TokenId::WHITE4:
                        index[whitesVerticalIndex] = (x << 3) | y;
                        ++whitesVerticalIndex;
                        break;

                    case TokenId::WHITE5:
                        index[tokenIndex] = (x << 3) | y;
                        break;

                    case TokenId::RED1:
                        index[tokenIndex] = (x << 3) | y;
                        break;

                    // Just to satisfy compiler
                    default:
                        break;
                }

                assigned[tokenIndex] = true;
            }
        }
    }

    tokenIndex = 0;

    do
    {
        id <<= 5;
        id |= index[tokenIndex] & 0x1F;
    }
    while (++tokenIndex < tokenCount);

    return id;
}

KPboard KPboard::CreateRootBoard()
{
    KPboard rootBoard;

    // Create a board with all tokens in start position
    rootBoard.InitializeToken(TokenId::GREEN1, 1, 3);
    rootBoard.InitializeToken(TokenId::GREEN2, 2, 3);
    rootBoard.InitializeToken(TokenId::GREEN3, 1, 4);
    rootBoard.InitializeToken(TokenId::GREEN4, 2, 4);
    rootBoard.InitializeToken(TokenId::WHITE1, 0, 0);
    rootBoard.InitializeToken(TokenId::WHITE2, 3, 0);
    rootBoard.InitializeToken(TokenId::WHITE3, 0, 3);
    rootBoard.InitializeToken(TokenId::WHITE4, 3, 3);
    rootBoard.InitializeToken(TokenId::WHITE5, 1, 2);
    rootBoard.InitializeToken(TokenId::RED1,   1, 0);

    return rootBoard;
}

