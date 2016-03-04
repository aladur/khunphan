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

#ifdef WIN32
// Extenstion for Win32: Scan command line parameters based
// on a static command line parameter string
static void scanCmdLine(LPSTR lpCmdLine, int *argc, char **argv)
{
    *argc = 1;
    *(argv + 0) = PACKAGE;
    while (*lpCmdLine)
    {
        *(argv + *argc) = lpCmdLine;
        while (*lpCmdLine && *lpCmdLine != ' ' && *lpCmdLine != '\t')
        {
            lpCmdLine++;
        }
        if (*lpCmdLine)
        {
            *(lpCmdLine++) = '\0';
        }
        while (*lpCmdLine && (*lpCmdLine == ' ' || *lpCmdLine == '\t'))
        {
            lpCmdLine++;
        }
        (*argc)++;
    }
} // scanCmdLine
#endif

int main (int argc, char **argv)
{
    KhunPhanApp application;
    int returnCode = EXIT_SUCCESS;

#ifdef HAVE_MCHECK_H
#ifdef CHECK_MEMORY_LEAKS
    mtrace();
#endif
#endif

#ifdef SIGINT
    (void)signal(SIGINT, _interrupt);
#endif

    if (application.Initialize(argc, argv))
    {
        application.Run(argc, argv);
        application.Shutdown();
    }

    KPConfig::Instance().finalize();

    /*
      BTexture in, ina, in1, out;

      const char *texels  = in.ReadTextureFromFile(
                                          "/home/spock/tmp/characters.png",
                                          TEX_WITH_ALPHA);
      const char *texelsa = ina.ReadTextureFromFile(
                                          "/home/spock/tmp/charactersalpha.png",
                                          TEX_WITH_ALPHA);
      if (texels != NULL && texelsa != NULL)
      {
        char *texels1 = (char *)new char [in.GetWidth() * in.GetHeight() * 2];
        unsigned int i;

        for (i = 0; i < in.GetWidth() * in.GetHeight(); i++)
        {
          texels1[2*i]   = texels[4*i];
          texels1[2*i+1] = (texels[4*i] != 0) ? 255 : texelsa[4*i];
        }

        out.SetTexels (texels1, in.GetWidth(), in.GetHeight(), 2,
                       TEX_ILLUMINANCE_ALPHA);
        if ( out.WriteTextureToFile("/home/spock/tmp/characters1.png") )
        {
            std::cout << "Write was successfull" << std::endl;
            in1.ReadTextureFromFile("/home/spock/tmp/characters1.png",
                                    TEX_WITH_ALPHA);
        }

        delete [] texels1;
      }

      //////////////////////////////////////////////////////////////////////////
      BTexture in, in1, out;

      const char *texels  = in.ReadTextureFromFile("/home/spock/tmp/logo.png",
                                                   TEX_WITH_ALPHA);
      if (texels != NULL)
      {
        char *texels1 = (char *)new char [in.GetWidth() * in.GetHeight() * 2];
        unsigned int i;

        for (i = 0; i < in.GetWidth() * in.GetHeight(); i++)
        {
          texels1[2*i+1] = texels[4*i];  // use red for alpha
          texels1[2*i]   = texels[4*i+1]; // use green for gray scale
        }

        out.SetTexels (texels1, in.GetWidth(), in.GetHeight(), 2,
                       TEX_ILLUMINANCE_ALPHA);
        if ( out.WriteTextureToFile("/home/spock/tmp/logo1.png") )
        {
            std::cout << "Write was successfull" << std::endl;
            in1.ReadTextureFromFile("/home/spock/tmp/logo1.png",
                                    TEX_WITH_ALPHA);
        }

        delete [] texels1;
      }
    */
    return returnCode;
}

#ifdef WIN32
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
    int   argc;
    char  *argv[50];

    scanCmdLine(lpCmdLine, &argc, (char **)argv);
    return main(argc, argv);
}
#endif
