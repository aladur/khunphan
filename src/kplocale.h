/*
    kplocale.h

    Automatic solution finder for KhunPhan game
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

#ifndef KPLOCALE_H
#define KPLOCALE_H

#include "language.h"
#include <map>
#include <string>


typedef std::map<Lbl, std::string> tIdToString;

class KPlocale
{
public:
    static tIdToString ReadFromFile(const std::string &filename);
    static const std::string FileExtension;
};

#endif
