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

#ifndef KHUNPHAN_H
#define KHUNPHAN_H

#include "stdafx.h"
#include <memory>
#include "kpboard.h"
#include "kpconfig.h"


class KPUIBase;


class KhunPhanApp
{
public:
    KhunPhanApp(int argc, char **argv);
    KhunPhanApp() = delete;
    KhunPhanApp(const KhunPhanApp &) = delete;
    KhunPhanApp &operator=(const KhunPhanApp &) = delete;
    ~KhunPhanApp();

    void Run();

private:
    bool Initialize();
    void InitializeSolutionTree();

    KPConfigPtr config;
    std::unique_ptr<KPUIBase> userInterfacePtr;
    int argc;
    char **argv;
    bool canRun;
};

#endif

