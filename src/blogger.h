/*
    blogger.h

    Basic class for logging messages
    Copyright (C) 2016-2018  W. Schwotzer

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

#ifndef BLOGGER_H
#define BLOGGER_H

#include "stdafx.h"
#include <sstream>


class BLogger
{
public:
    static void SetOutputStream(std::ostream &os)
    {
        pOutput = &os;
    }
#ifdef _WIN32
    static void SetDebugOutput(bool on)
    {
        isDebugOutput = on;
    }
#endif

    static void SetPrefix(const std::string &pprefix)
    {
        prefix = pprefix;
    }

    template<typename... Args>
    static void Log(Args&&... args)
    {
        std::stringstream str;

        str << prefix;

        sslog(str, args...);

        str << std::endl;

        if (isDebugOutput)
        {
#ifdef _WIN32
            OutputDebugString(static_cast<LPCTSTR>(str.str().c_str()));
#endif
        }
        else
        {
            *pOutput << str.str();
        }
    }

    // Logging any parameter into a stringstream using variadic templates.
    // End of recursion.
    template<typename T>
    static void sslog(std::stringstream &str, T p1)
    {
        str << p1;
    }

    // Logging any parameter into a stringstream using variadic templates.
    template<typename T, typename... Args>
    static void sslog(std::stringstream &str, T p1, Args&&... args)
    {
        str << p1;
        sslog(str, args...);
    }

private:
    static std::ostream *pOutput;
    static std::string  prefix;
    static bool isDebugOutput;
};
#endif

