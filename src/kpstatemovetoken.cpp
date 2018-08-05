/*
    kpstatemovetoken.cpp

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
#include "kpstatemovetoken.h"
#include "kpmenu.h"
#include "kpboardGL.h"


KPstateMoveToken::KPstateMoveToken() : mouse_x(0), mouse_y(0)
{
}

// returns true if event has been processed
bool KPstateMoveToken::MouseMoveToken(KPstateContext *pContext,
                                      MouseButton button,
                                      MouseButtonEvent event,
                                      int x, int y)
{
    static auto token = TokenId::EMPTY;

    if (button == MouseButton::Left && event == MouseButtonEvent::Press)
    {
        // Pressing left menu button: remind token
        token = pContext->GetBoardView().Selection(pContext->GetCamera(),
                x, y);
        mouse_x = x; // save current mouse position
        mouse_y = y;
        return true;
    }
    else if (button == MouseButton::Left && event == MouseButtonEvent::Release)
    {
        // Releasing left mouse button: move token
        if (token != TokenId::EMPTY)
        {
            // calculate mouse direction
            MoveToken direction = MoveToken::Not;
            auto diff_x = x - mouse_x;
            auto diff_y = y - mouse_y;

            if (abs(diff_x) - abs(diff_y) > 1)
            {
                // Move left or right
                direction = (diff_x > 0) ? MoveToken::Left : MoveToken::Right;
            }
            else if (abs(diff_y) - abs(diff_x) > 1)
            {
                // Move up or down
                direction = (diff_y > 0) ? MoveToken::Down : MoveToken::Up;
            }

            if (HookDoTheMove(pContext, token, direction))
            {
                bool result = pContext->GetBoardView().Move(token, direction);

                HookAfterTokenMoved(pContext, token, direction, result);
            }
        }
        else
        {
            HookAfterTokenMoved(pContext, TokenId::EMPTY,
                                MoveToken::Not, false);
        }

        return true;
    }
    else
    {
        token = TokenId::EMPTY;
        return false;
    }
}

bool KPstateMoveToken::HookDoTheMove(KPstateContext *, TokenId, MoveToken)
{
    // default: always do the move
    return true;
}

void KPstateMoveToken::HookAfterTokenMoved(KPstateContext *, TokenId,
        MoveToken, bool)
{
    // default: do nothing
}
