/*
    kpsolutionscountiterator.h


    Copyright (C) 2016-2024  W. Schwotzer

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

#ifndef KPSOLUTIONSCOUNTFUNCTION_H
#define KPSOLUTIONSCOUNTFUNCTION_H


class KPsolutionsCountFunction
{
public:
    KPsolutionsCountFunction() : count(0)
    {
    }

    inline void Iterate(KPnode &node)
    {
        if (node.GetBoard().IsSolved())
        {
            ++count;
        }
    };

    inline int GetCount() const
    {
        return count;
    }

private:
    int count;
};

#endif

