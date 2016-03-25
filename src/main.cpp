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

// Uncomment the following define to activate GNU C library memory leak
// checking.
// Run khunphan with memory leak checking (don't strip khunphan executable):
// $ MALLOC_TRACE=mtrace.log ./khunphan
// Output memory leak check results:
// $ mtrace ./khunphan mtrace.log
// See also: https://en.wikipedia.org/wiki/Mtrace
//
//#define CHECK_MEMORY_LEAKS

int main (int argc, char **argv)
{
#if defined(HAVE_MCHECK_H) && defined(CHECK_MEMORY_LEAKS)
    mtrace();
#endif

    try {
        KhunPhanApp application(argc, argv);

        application.Run();

    } catch (std::exception &e)
    {
        message(mtError, "%s.\nProgram terminates now.", e.what());
    }

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

    CmdLine::Scan(lpCmdLine, &argc, (char **)argv,
                  sizeof(argv) / sizeof(argv[0]));
    return main(argc, argv);
}
#endif
