/*
    misc1.cpp


    Copyright (C) 2001-2024  W. Schwotzer

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


TokenId &operator++ (TokenId &id)
{
    if (id == TokenId::EMPTY)
    {
        return id;
    }

    if (id == TokenId::RED1)
    {
        id = TokenId::GREEN1;
    }
    else
    {
        auto value = static_cast<std::size_t>(id);

        id = static_cast<TokenId>(value + 1);
    }

    return id;
}

MoveToken &operator++ (MoveToken &id)
{
    if (id == MoveToken::Right)
    {
        id = MoveToken::Up;
    }
    else
    {
        int value = static_cast<int>(id);

        id = static_cast<MoveToken>(value + 1);
    }

    return id;
}

