/*
    cmdline.cpp

    Automatic solution finder for KhunPhan game
    Copyright (C) 2016  W. Schwotzer

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
#include "cmdline.h"

#ifdef _WIN32
// Extension for Win32: Scan command line parameters based
// on a static command line parameter string
void CmdLine::Scan(LPSTR lpCmdLine, int *argc, char **argv, int max_argc)
{
    *argc = 1;
    *(argv + 0) = PACKAGE;

    while (*lpCmdLine && (*argc < max_argc))
    {
        *(argv + *argc) = lpCmdLine;

        while (*lpCmdLine && *lpCmdLine != ' ' && *lpCmdLine != '\t')
        {
            lpCmdLine++;
        }

        if (*lpCmdLine)
        {
            *(lpCmdLine++) = '\0';
        }

        while (*lpCmdLine && (*lpCmdLine == ' ' || *lpCmdLine == '\t'))
        {
            lpCmdLine++;
        }

        (*argc)++;
    }
}

#endif
