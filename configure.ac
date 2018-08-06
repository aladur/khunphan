dnl Process this file with autoconf to produce a configure script.

AC_INIT([khunphan],0.61)
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR(m4)
AC_CANONICAL_TARGET
AC_ARG_PROGRAM dnl Perform program name transformation
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AM_MAINTAINER_MODE([disable])
dnl AC_PREFIX_DEFAULT(/usr/local)
AM_CONFIG_HEADER([src/config.h])
AC_PROG_LN_S
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_MAKE_SET

dnl check for OpenGL and GLU
AX_CHECK_GLU
if test "x$no_glu" = "xyes"; then
   AC_MSG_ERROR([*** No OpenGL or GLU library found!])
fi

dnl check for GLEW
AX_CHECK_GLEW

dnl check for GLUT
AX_CHECK_GLUT
if test "x$no_glut" != "xyes"; then
   AC_DEFINE(HAVE_LIBGLUT, 1, [Define to 1 if library and header file for libglut is available.])
fi

dnl Check for SDL V2.0
SDL2_VERSION=2.0.0
have_sdl2=yes
AM_PATH_SDL2($SDL2_VERSION,
            AC_DEFINE([HAVE_SDL2], 1, [Define to 1 if library and header file for libsdl2 is available.]),
         have_sdl2=""
)

dnl Check if SDL2 is available but no GLEW
if test "x$have_sdl2" = xyes -a "x$no_glew" = xyes; then
 AC_MSG_ERROR([*** SDL2 needs GLEW library too!])
fi

dnl Check for SDL2_mixer
if test "x$have_sdl2" = "xyes"; then
 AC_CHECK_LIB(SDL2_mixer,
        main, 
        LIBS="$LIBS -lSDL2_mixer",
        AC_MSG_ERROR([*** SDL2_mixer not found!])
 )
fi

if test "x$have_sdl2" != "xyes"; then
 dnl If SDL V2.0 is not present check for SDL V1.2
 SDL_VERSION=1.2.0
 AM_PATH_SDL($SDL_VERSION,
             AC_DEFINE([HAVE_SDL], 1, [Define to 1 if library and header file for libsdl is available.]),
             AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!])
 )
 dnl Check for SDL_mixer
 AC_CHECK_LIB(SDL_mixer,
        main,
        LIBS="$LIBS -lSDL_mixer",
        AC_MSG_ERROR([*** SDL_mixer library not found!])
 )
fi

 
CXXFLAGS="$CXXFLAGS $SDL_CFLAGS $GLEW_CFLAGS"
LIBS="$LIBS $SDL_LIBS $GLEW_LIBS"

dnl Check for libxml2
AM_PATH_XML2
if test "x$no_xml" = "xyes"; then
   AC_MSG_ERROR([*** No libxml2 library found!])
fi

dnl Check for libpng
AM_PATH_LIBPNG
if test "x$no_png" = "xyes"; then
   AC_MSG_ERROR([*** No libpng library found!])
fi

AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_TIME

AC_CHECK_HEADERS(sys/time.h sys/types.h sys/stat.h)
AC_CHECK_HEADERS(string.h strings.h stdlib.h stdarg.h)
AC_CHECK_HEADER(unistd.h)
AC_CHECK_HEADERS(mcheck.h)

AC_CONFIG_FILES([ Makefile ])
AC_CONFIG_FILES([ src/Makefile ])

AC_OUTPUT

if test "x$no_glut" = "xyes"; then
echo
echo "Info: Building khunphan without libglut"
echo
fi
