/*
    KhunPhan.h


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

#ifndef __khunphan_h__
#define __khunphan_h__

#include <string>
#include "kpboard.h"

class KPUIBase;


class KhunPhanApp
{
public:
    KhunPhanApp();
    ~KhunPhanApp();

    bool Initialize(int argc, char **argv);
    void InitializeSolutionTree();
    void Run(int argc, char **argv);
    void Shutdown();
    const char *GetAppName()
    {
        return appName.c_str();
    };

    KPboard rootBoard;

private:
    std::string appName;
    KPUIBase *userInterface;
};

#endif

