/*
    misc1.cpp


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
#include <ctype.h>
#include <string>
#include "sprinter.h"


void message(tMsgType type, const char *format, ...)
{
    va_list arg_ptr;
    std::string msg;

    va_start(arg_ptr, format);
    sprinter::vsprintf(msg, format, arg_ptr);
    va_end(arg_ptr);
#ifdef LINUX
    switch (type)
    {
        case mtError:
            std::cerr << msg << std::endl;
            break;
        case mtMessage:
            std::cout << msg << std::endl;
            break;
        case mtWarning:
            std::cerr << msg << std::endl;
            break;
    }
#else
#ifdef WIN32
    std::string msg1;
    sprinter::sprintf(msg1, "[%s] %s\n", PACKAGE, msg.c_str());
    OutputDebugString(msg1.c_str());
    switch (type)
    {
        case mtError:
            MessageBox(NULL, msg.c_str(), PACKAGE, MB_OK | MB_ICONERROR);
            break;
        case mtMessage:
            MessageBox(NULL, msg.c_str(), PACKAGE, MB_OK);
            break;
        case mtWarning:
            MessageBox(NULL, msg.c_str(), PACKAGE, MB_OK | MB_ICONEXCLAMATION);
            break;
    }
#else
#error "Unsupported platform"
#endif
#endif
}

tKPTokenID &operator++ (tKPTokenID &d)
{
    if (d == TK_EMPTY)
    {
        return d;
    }
    return d = ((d == TK_RED1) ? TK_GREEN1 : tKPTokenID(d + 1));
}

tKPDirection &operator++ (tKPDirection &d)
{
    return d = ((d == MOVE_RIGHT) ? MOVE_UP : tKPDirection(d + 1));
}

