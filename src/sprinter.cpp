/*
    sprinter.cpp


    Copyright (C) 2015-2016  W. Schwotzer

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
#include <stdio.h>
#include <stdexcept>
#include "sprinter.h"

#define MAX_BUFFER_SIZE   (2048)

// This utility class supports printing into a std::string with a
// variable argument list. It uses the old style va_arg, va_start
// and va_end and thus it is not type safe.
//
// As a difference to the standard sprintf and vsprintf functions
// if the buffer was not large enough a length error exception is returned.
// It would be possible to dynamically allocate the buffer and cyclically
// increase it's size until it's large enough. But in this context
// it is sufficient to have one try.
// The exception is also thrown by an unexpected compiler.
//
// In c++11 this can be implemented in a type safe way using
// variadic templates.

int sprinter::sprintf(std::string &s, const char *format, ...)
{
    va_list arg_ptr;
    char buffer[MAX_BUFFER_SIZE];
    char *p = static_cast<char *>(buffer);
    int res = -1;

    if (format == NULL || strlen(format) == 0)
    {
        s.clear();
        return 0;
    }

    va_start(arg_ptr, format);
#ifdef _MSC_VER
    res = _vsnprintf_s(p, MAX_BUFFER_SIZE, MAX_BUFFER_SIZE - 1,
                       format, arg_ptr);
#else
#ifdef __GNUC__
    res = vsnprintf(p, MAX_BUFFER_SIZE, format, arg_ptr);
#else
#error "Unsupported platform"
#endif
#endif
    va_end(arg_ptr);

    if (res < 0 || (res >= MAX_BUFFER_SIZE))
    {
        throw std::length_error("sprinter::sprintf buffer too short.");
    }

    s = p;

    return res;
}

int sprinter::vsprintf(std::string &s,
                       const char *format,
                       va_list arg_ptr)
{
    char buffer[MAX_BUFFER_SIZE];
    char *p = static_cast<char *>(buffer);
    int res = -1;

    if (format == NULL || strlen(format) == 0)
    {
        s.clear();
        return 0;
    }

#ifdef _MSC_VER
    res = _vsnprintf_s(p, MAX_BUFFER_SIZE, MAX_BUFFER_SIZE - 1,
                       format, arg_ptr);
#else
#ifdef __GNUC__
    res = vsnprintf(p, MAX_BUFFER_SIZE, format, arg_ptr);
#else
#error "Unsupported platform"
#endif
#endif

    if (res < 0 || (res >= MAX_BUFFER_SIZE))
    {
        throw std::length_error("sprinter::vsprintf buffer too short.");
    }

    s = p;

    return res;
}

#undef MAX_BUFFER_SIZE

bool sprinter::isformatstring(const std::string &text)
{
    std::string::size_type pos = 0;

    while (pos != std::string::npos)
    {
        pos = text.find('%', pos);

        if (pos != std::string::npos)
        {
            ++pos;

            if (text[pos] != '\0' && text[pos] != '%')
            {
                return true;
            }

            ++pos;
        }
    }

    return false;
}

