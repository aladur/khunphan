/*
    kplocale.cpp

    Automatic solution finder for KhunPhan game
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
#include <fstream>
#include "kplocale.h"


KPlocale::KPlocale() 
{
}

KPlocale::KPlocale(const std::string &filename)
{
    std::ifstream ifs(filename.c_str());
    std::string text;
    int id;

    if (!ifs)
    {
        // Error reading the file is silently ignored here
        return;
    }

    while (!ifs.eof())
    {
        if (!(ifs >> id))
        {
            ifs.clear();
            std::getline(ifs, text);
            continue;
        }

        std::getline(ifs, text);
        if (text[0] == ' ' || text[0] == '\t')
        {
            text.erase(0, 1);
        }

        strings[id] = text;
    }

    ifs.close();
}

