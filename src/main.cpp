/*
    main.cpp

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
#include <signal.h>
#include <ostream>
#ifdef HAVE_MCHECK_H
#include <mcheck.h>
#endif
#include "kpconfig.h"
#include "kpscore.h"
#include "cmdline.h"
#include "KhunPhan.h"

// uncomment the following line to activate memory leak checking
// memory leak checking is used in the following way:
// (dont strip khunphan)
// $ MALLOC_TRACE=mlog khunphan
// $ mtrace khunphan mlog
//#define CHECK_MEMORY_LEAKS

//#ifndef WIN32
#define RETSIGTYPE void

extern "C" RETSIGTYPE _interrupt(int)
{
    LOG1(PACKAGE " aborted by user");
    exit(0);
}
//#endif

int main (int argc, char **argv)
{
    KhunPhanApp *application = new KhunPhanApp();

#ifdef HAVE_MCHECK_H
#ifdef CHECK_MEMORY_LEAKS
    mtrace();
#endif
#endif

#ifdef SIGINT
    (void)signal(SIGINT, _interrupt);
#endif

    if (application->Initialize(argc, argv))
    {
        application->Run(argc, argv);
    }

    delete application;

    KPConfig::Instance().WriteToFile();
    KPConfig::Instance().finalize();
    KPscore::Instance().finalize();

    return EXIT_SUCCESS;
}

#ifdef WIN32
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
    int   argc;
    char  *argv[50];

    CmdLine::Scan(lpCmdLine, &argc, (char **)argv);
    return main(argc, argv);
}
#endif
