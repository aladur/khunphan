/*
    bdir.h


    Basic class used for directory functions
    Copyright (C) 1999-2018  W. Schwotzer

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

#ifndef BDIR_H
#define BDIR_H

#include "stdafx.h"
#include <vector>
#include <string>


typedef std::vector<std::string> tPathList;

class BDirectory
{
public:
    static bool Exists(const std::string &aPath);
    static bool Remove(const std::string &aPath);
    static bool Create(const std::string &aPath, int mode = 0x0755);
    static bool RemoveRecursive(const std::string &aPath);
    static tPathList GetSubDirectories(const std::string &aPath);
    static tPathList GetFiles(const std::string &aPath);
};

#endif
