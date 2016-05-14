/*
    misc1.h


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

#ifndef MISC1_H
#define MISC1_H

// For best possible portability the following header files only should be
// included within here. If one of these files is needed to include just
// include misc1.h instead
//  <string(s)>.h
//  <stdlib.h>

#ifndef _WIN32
    #include "config.h"
#endif

#if HAVE_INTTYPES_H
    #include <inttypes.h>
#else
    #if HAVE_STDINT_H
        #include <stdint.h>
    #endif
#endif

/* time */
#if TIME_WITH_SYS_TIME
    #include <sys/time.h>
    #include <time.h>
#else
    #if HAVE_SYS_TIME_H
        #include <sys/time.h>
    #else
        #include <time.h>
    #endif
#endif

#ifdef _WIN32
    /* Name of package */
    #define PACKAGE "khunphan"

    /* Version number of package */
    #define VERSION "0.60"

    #define HAVE_SDL2        1
    #define HAVE_LIBGLUT     1
    //#define HAVE_LIBOPENGLUT 1
    #define HAVE_STRING_H    1
    #define HAVE_SYS_STAT_H  1
    #define HAVE_SYS_TYPES_H 1
    #define HAVE_STDLIB_H    1
    #define HAVE_STDARG_H    1
#endif // #ifdef _WIN32

#ifdef HAVE_STRING_H
    #include <string.h>
#else
    #ifdef HAVE_STRINGS_H
        #include <strings.h>
    #endif
#endif

#ifdef HAVE_STDLIB_H
    #include <stdlib.h>
#endif

#include <sstream>
#include <iostream>
#include <iomanip>
#include "blogger.h"

/* PATHSEPARATORSTRING shoud be a define to do */
/* implicit concatenation by the compiler!     */
#ifdef _WIN32
    const char PATHSEPARATOR = '\\';
    #define PATHSEPARATORSTRING  "\\"
#else
    const char PATHSEPARATOR = '/';
    #define PATHSEPARATORSTRING "/"
#endif

#ifdef _MSC_VER
    typedef unsigned __int64 QWord;
    typedef __int64 SQWord;
#else
    #if defined(HAVE_INTTYPES_H) || defined(HAVE_STDINT_H)
        typedef uint64_t QWord;
        typedef int64_t SQWord;
    #else
        #if (SIZEOF_LONG == 8)
            typedef unsigned long QWord;
            typedef long SQWord;
        #else
            #if (SIZEOF_LONG_LONG == 8)
                typedef unsigned long long QWord;
                typedef long long SQWord;
            #else
                /* a class with opterator overloading could help here */
                #error No data type of size 8 found
            #endif
        #endif
    #endif
#endif /* #ifdef _MSC_VER */

#ifdef _MSC_VER
    #define R_OK            (4) /* read  permission */
    #define W_OK            (2) /* write permission */
    #define F_OK            (0) /* file existence   */
    #define S_ISDIR(x)      (x & S_IFDIR)
    #define S_ISREG(x)      (x & S_IFREG)
    #define access          _access
#endif

#define ON_OFF(b) ((b) ? "On" : "Off")

enum tKPTokenID
{
    TK_EMPTY    = 99,
    TK_GREEN1   = 0,
    TK_GREEN2   = 1,
    TK_GREEN3   = 2,
    TK_GREEN4   = 3,
    TK_WHITE1   = 4, // vertical
    TK_WHITE2   = 5, // vertical
    TK_WHITE3   = 6, // vertical
    TK_WHITE4   = 7, // vertical
    TK_WHITE5   = 8, // horizontal
    TK_RED1     = 9
};

enum class MoveToken
{
    Up,
    Down,
    Left,
    Right,
    Not
};

enum class AlignItem
{
    DontCare,
    Left,
    Centered,
    Right
};

enum class EventCounter
{
    Moves,
    MovesWithHint,
    MovesWithCheat
};

enum class MouseButtonEvent
{
    Press,
    Release
};

enum class MouseButton
{
    Left,
    Middle,
    Right
};

#define MOD_FADEOUT      0.0f
#define MOD_TRANSPARENCY 0.2f
#define MOD_FADEIN       0.6f
#define MOD_SELECTED     0.8f
#define MOD_FULLYVISIBLE 1.0f

enum
{
    TOTAL_ANIMATIONTIME = 1000
};

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif
#ifndef M_PIf
    #define M_PIf 3.14159265f
#endif

extern tKPTokenID &operator++ (tKPTokenID &d);
extern MoveToken &operator++ (MoveToken &d);

typedef enum
{
    mtError   = 1,
    mtWarning = 2,
    mtMessage = 3
} tMsgType;

template<typename... Args>
void message(tMsgType type, Args&&... args)
{
    std::stringstream msg;

    BLogger::sslog(msg, args...);

#ifdef __linux__

    switch (type)
    {
        case mtError:
            std::cerr << "*** Error: " << msg.str() << std::endl;
            break;

        case mtMessage:
            std::cout << msg.str() << std::endl;
            break;

        case mtWarning:
            std::cerr << "*** Warning: " << msg.str() << std::endl;
            break;
    }

#else
#ifdef _WIN32
    std::stringstream winmsg;

    winmsg << "[" PACKAGE "] " << msg.str() << std::endl;
    OutputDebugString(winmsg.str().c_str());

    switch (type)
    {
        case mtError:
            MessageBox(nullptr, msg.str().c_str(), PACKAGE,
                       MB_OK | MB_ICONERROR);
            break;

        case mtMessage:
            MessageBox(nullptr, msg.str().c_str(), PACKAGE, MB_OK);
            break;

        case mtWarning:
            auto type = MB_OK | MB_ICONEXCLAMATION;
            MessageBox(nullptr, msg.str().c_str(), PACKAGE, type);
            break;
    }

#else
#error "Unsupported platform"
#endif
#endif
}

#endif

