/*
    misc1.h


    Copyright (C) 2001-2015  W. Schwotzer

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

#ifndef __misc1_h__
#define __misc1_h__

// For best possible portability the following header files only should be
// included within here. If one of these files is needed to include just
// include misc1.h instead
//  <string(s)>.h
//  <stdlib.h>

#ifndef WIN32
#include "config.h"
#endif

#if HAVE_INTTYPES_H
# include <inttypes.h>
#else
# if HAVE_STDINT_H
#  include <stdint.h>
# endif
#endif

/* time */
#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#ifdef WIN32
/* Name of package */
#define PACKAGE "khunphan"

/* Version number of package */
#define VERSION "0.56"

#define HAVE_SDL2        1
#define HAVE_LIBGLUT     1
//#define HAVE_LIBOPENGLUT 1
#define HAVE_STRING_H    1
#define HAVE_SYS_STAT_H  1
#define HAVE_SYS_TYPES_H 1
#define HAVE_STDLIB_H    1
#define HAVE_STDARG_H    1
#endif // #ifdef WIN32

#ifndef UNIX
#if defined(LINUX) || defined(__LINUX) || defined(__BSD) || defined(__SOLARIS)
#define UNIX
#endif
#endif

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

/* PATHSEPARATORSTRING shoud be a define to do */
/* implicit concatenation by the compiler!     */
#ifdef WIN32
const char PATHSEPARATOR = '\\';
#define PATHSEPARATORSTRING  "\\"
#else
const char PATHSEPARATOR = '/';
#define PATHSEPARATORSTRING "/"
#endif

#ifdef _MSC_VER
typedef unsigned __int64 QWord;
typedef __int64 SQWord;
#define FMT_UINT64x "%I64x"
#else
#if defined(HAVE_INTTYPES_H) || defined(HAVE_STDINT_H)
typedef uint64_t                QWord;
typedef int64_t                 SQWord;
#define FMT_UINT64x "%" PRIx64
#else
#if (SIZEOF_LONG == 8)
typedef unsigned long         QWord;
typedef long                  SQWord;
#define FMT_UINT64x "%lx"
#else
#if (SIZEOF_LONG_LONG == 8)
typedef unsigned long long  QWord;
typedef long long           SQWord;
#define FMT_UINT64x "%llx"
#else
/* a class with opterator overloading could help here */
#error No data type of size 8 found
#endif
#endif
#endif
#endif /* #ifdef _MSC_VER */

#ifdef LINUX
int stricmp(const char *string1, const char *string2);
#endif

#ifdef _MSC_VER
#define R_OK            (4) /* read  permission */
#define W_OK            (2) /* write permission */
#define F_OK            (0) /* file existence   */
#define S_ISDIR(x)      (x & S_IFDIR)
#define S_ISREG(x)      (x & S_IFREG)
#define set_new_handler _set_new_handler
#define access          _access
#define UNUSED(param)
#else
#define UNUSED(param) param
#endif

#define ON_OFF(b) ((b) ? "On" : "Off")

#ifdef WIN32
#define LOCALLOG(p) OutputDebugString(static_cast<LPCTSTR>(p.str().c_str()))
#else
#if defined (LINUX)
#define LOCALLOG(p) std::cout << p.str() << std::endl
#else
#error "Unsupported platform"
#endif
#endif


#define LOG1(p1)                                         \
  {                                                      \
    std::stringstream tmpstr;                            \
    tmpstr << "[" PACKAGE "] " << p1;                    \
    LOCALLOG(tmpstr);                                    \
  }
#define LOG2(p1,p2)                                      \
  {                                                      \
    std::stringstream tmpstr;                            \
    tmpstr << "[" PACKAGE "] " << p1 << p2;              \
    LOCALLOG(tmpstr);                                    \
  }
#define LOG3(p1,p2,p3)                                   \
  {                                                      \
    std::stringstream tmpstr;                            \
    tmpstr << "[" PACKAGE "] " << p1 << p2 << p3;        \
    LOCALLOG(tmpstr);                                    \
  }
#define LOG4(p1,p2,p3,p4)                                \
  {                                                      \
    std::stringstream tmpstr;                            \
    tmpstr << "[" PACKAGE "] " << p1 << p2 << p3 << p4;  \
    LOCALLOG(tmpstr);                                    \
  }
#define LOG5(p1,p2,p3,p4,p5)                             \
  {                                                      \
    std::stringstream tmpstr;                            \
    tmpstr << "[" PACKAGE "] " << p1 << p2 << p3 << p4   \
                               << p5;                    \
    LOCALLOG(tmpstr);                                    \
  }
#define LOG6(p1,p2,p3,p4,p5,p6)                          \
  {                                                      \
    std::stringstream tmpstr;                            \
    tmpstr << "[" PACKAGE "] " << p1 << p2 << p3 << p4   \
                               << p5 << p6;              \
    LOCALLOG(tmpstr);                                    \
  }
#define LOG7(p1,p2,p3,p4,p5,p6,p7)                       \
  {                                                      \
    std::stringstream tmpstr;                            \
    tmpstr << "[" PACKAGE "] " << p1 << p2 << p3 << p4   \
                               << p5 << p6 << p7;        \
    LOCALLOG(tmpstr);                                    \
  }
#define LOG8(p1,p2,p3,p4,p5,p6,p7,p8)                    \
  {                                                      \
    std::stringstream tmpstr;                            \
    tmpstr << "[" PACKAGE "] " << p1 << p2 << p3 << p4   \
                               << p5 << p6 << p7 << p8;  \
    LOCALLOG(tmpstr);                                    \
  }
#define LOG9(p1,p2,p3,p4,p5,p6,p7,p8,p9)                 \
  {                                                      \
    std::stringstream tmpstr;                            \
    tmpstr << "[" PACKAGE "] " << p1 << p2 << p3 << p4   \
                               << p5 << p6 << p7 << p8   \
                               << p9;                    \
    LOCALLOG(tmpstr);                                    \
  }

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

enum tKPDirection
{
    MOVE_UP    = 1,
    MOVE_DOWN  = 2,
    MOVE_LEFT  = 4,
    MOVE_RIGHT = 8,
    MOVE_NO    = 15
};

enum tKPAlignment
{
    A_EGAL,
    A_LINKS,
    A_MITTE,
    A_RECHTS
};

enum tEventCounter
{
    MOVE_COUNTER,
    MOVE_WITH_HELP_CNT,
    USED_CHEATS_CNT
};

enum tMouseEvent
{
    KP_BUTTON_RELEASE  = 44,
    KP_BUTTON_PRESS    = 45,
};

enum tMouseButton
{
    KP_LEFT_MB         = 46,
    KP_MIDDLE_MB       = 47,
    KP_RIGHT_MB        = 48
};

#define MOD_AUSGEBLENDET 0.0f
#define MOD_TRANSPARENT  0.2f
#define MOD_EINGEBLENDET 0.6f
#define MOD_ANGEWAEHLT   0.8f
#define MOD_VOLLSICHTBAR 1.0f

enum
{
    TOTAL_ANIMATIONTIME = 100
};

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_PIf
#define M_PIf 3.14159265f
#endif

extern tKPTokenID &operator++ (tKPTokenID &d);
extern tKPDirection &operator++ (tKPDirection &d);

typedef enum
{
    mtError   = 1,
    mtWarning = 2,
    mtMessage = 3
} tMsgType;

void message(tMsgType, const char *format, ...);

#endif

