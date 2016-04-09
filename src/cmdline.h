/*
    cmdline.h

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

#ifndef CMDLINE_H
#define CMDLINE_H

#ifdef _WIN32
class CmdLine
{
public:
    static void Scan(LPSTR lpCmdLine, int *argc, char **argv, int max_args);
};

#endif
#endif
