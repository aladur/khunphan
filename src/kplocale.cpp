/*
    kplocale.cpp

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

#include "stdafx.h"
#include <fstream>
#include "kplocale.h"


const std::string KPlocale::FileExtension = ".lang";

tIdToString KPlocale::ReadFromFile(const std::string &filename)
{
    std::ifstream ifs(filename.c_str());
    tIdToString strings;

    // An error when reading the file is silently ignored here

    if (ifs.is_open())
    {
        while (!ifs.eof())
        {
            std::string text;
            int id;

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

            // Check if text is continued on next line with a trailing '\\'
            while (!text.empty() && (text[text.size() - 1] == '\\'))
            {
                text.erase(text.size() - 1);

                if (!ifs.eof())
                {
                    std::string nextline;

                    std::getline(ifs, nextline);
                    text.append(nextline);
                }
            }

            strings[static_cast<Lbl>(id)] = text;
        }

        ifs.close();
    }

    return strings;
}

