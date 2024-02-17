/*
    misc1.h


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

#ifndef MISC1_H
#define MISC1_H

// For best possible portability the following header files only should be
// included within here. If one of these files is needed to include just
// include misc1.h instead
//  <string(s)>.h
//  <stdlib.h>

#include <cstdint>

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
#if HAVE_SYS_TIME_H
    #include <sys/time.h>
#endif
#include <time.h>

#ifdef _WIN32
    /* Name of package */
    #define PACKAGE "khunphan"

    /* Version number of package */
    #define VERSION "0.62"

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

enum class TokenId : uint8_t
{
    GREEN1,
    GREEN2,
    GREEN3,
    GREEN4,
    WHITE1, // vertical
    WHITE2, // vertical
    WHITE3, // vertical
    WHITE4, // vertical
    WHITE5, // horizontal
    RED1,
    COUNT, // Should be the last one but before EMPTY
    EMPTY = 99,
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

constexpr int TOTAL_ANIMATIONTIME = 1000;

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif
#ifndef M_PIf
    #define M_PIf 3.14159265f
#endif

extern TokenId &operator++ (TokenId &d);
extern MoveToken &operator++ (MoveToken &d);

enum class MsgType
{
    Error,
    Warning,
    Info
};

template<typename... Args>
void message(MsgType type, Args&&... args)
{
    std::stringstream msg;

    BLogger::sslog(msg, args...);

#ifdef __linux__

    switch (type)
    {
        case MsgType::Error:
            std::cerr << "*** Error: " << msg.str() << std::endl;
            break;

        case MsgType::Info:
            std::cout << msg.str() << std::endl;
            break;

        case MsgType::Warning:
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
        case MsgType::Error:
            MessageBox(nullptr, msg.str().c_str(), PACKAGE,
                       MB_OK | MB_ICONERROR);
            break;

        case MsgType::Info:
            MessageBox(nullptr, msg.str().c_str(), PACKAGE, MB_OK);
            break;

        case MsgType::Warning:
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

