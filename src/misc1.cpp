/*
    misc1.cpp
 
 
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

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <ctype.h>
#ifdef HAVE_STDARG_H
  #include <stdarg.h>
#endif
#include <stdio.h>
#include "misc1.h"
#include "bstring.h"


void message(tMsgType type, const char *format, ...)
{
	va_list arg_ptr;
	BString msg;

	va_start(arg_ptr, format);
	msg.vsprintf(format, arg_ptr);
	va_end(arg_ptr);
#ifdef LINUX
	switch (type) {
	case mtError:   fprintf(stderr, "%s", (const char *)msg); break;
	case mtMessage: fprintf(stdout, "%s", (const char *)msg); break;
	case mtWarning: fprintf(stderr, "%s", (const char *)msg); break;
	}
#endif
#ifdef WIN32
	BString msg1;
	msg1.printf("[%s] %s\n", PACKAGE, msg);
	OutputDebugString(msg1);
	switch (type) {
	case mtError:   MessageBox(NULL, (LPCTSTR)msg, PACKAGE, MB_OK | MB_ICONERROR); break;
	case mtMessage: MessageBox(NULL, (LPCTSTR)msg, PACKAGE, MB_OK); break;
	case mtWarning: MessageBox(NULL, (LPCTSTR)msg, PACKAGE, MB_OK | MB_ICONEXCLAMATION); break;
	}
#endif
}

#ifndef WIN32
int stricmp(const char *string1, const char *string2)
{
        unsigned int i;
 
        for (i = 0; i < strlen(string1); i++) {
                if (tolower(*(string1 + i)) < tolower(*(string2 + i)))
                        return -1;
                if (tolower(*(string1 + i)) > tolower(*(string2 + i)))
                        return 1;
                if (!*string1)
                        return 0;
        }
        return 0;
} // stricmp
#endif                                                                          

tKPTokenID& operator++ (tKPTokenID& d)
{
	if (d == TK_EMPTY)
		return d;
	return d = ((d == TK_RED1) ? TK_GREEN1 : tKPTokenID(d + 1));	
}

tKPDirection& operator++ (tKPDirection& d)
{
	return d = ((d == MOVE_RIGHT) ? MOVE_UP : tKPDirection(d + 1));	
}

