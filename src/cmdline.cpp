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

#include "cmdline.h"

#ifdef WIN32
// Extension for Win32: Scan command line parameters based
// on a static command line parameter string
static void CmdLine::Scan(LPSTR lpCmdLine, int *argc, char **argv)
{
    *argc = 1;
    *(argv + 0) = PACKAGE;
    while (*lpCmdLine)
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
