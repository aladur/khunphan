/*
    sprinter.h


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

#ifndef __SPRINTER_H__
#define __SPRINTER_H__


#include "misc1.h"

#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif
#include <string>

class sprinter
{
public:
    static int sprintf(std::string &s, const char *format, ...);
    static int vsprintf(std::string &s, const char *format,
                        va_list arg_ptr);
    static bool isformatstring(const std::string &text);
};

#endif

