/*
    bstring.cpp


    Basic class containing a string implementation
    Copyright (C) 1998-2015  W. Schwotzer

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

#include "misc1.h"
#include <ctype.h>
#include <stdio.h>
#include <limits.h>

#include "bstring.h"


#ifdef __GNUC__
//extern int vsnprintf(char *buf, size_t size, const char *format, va_list args);
#endif

void BString::initForChar(const char c)
{
	sz = 2;
	str = new char[sz];
	str[0] = c;
	str[1] = '\0';
}

void BString::init(const char *s, int maxlen)
{
	if (maxlen == 0 || s == NULL) {
		initForChar('\0');
		return;
	}
	if (maxlen < 0)
		sz = strlen(s) + 1;
	else
		sz = maxlen + 1;
	if (sz < 2)
		sz = 2;

  char *pTmp = str;
	str = new char[sz];
	strncpy(str, s, sz-1);
	str[sz-1] = '\0';
  delete [] pTmp;
}

BString::BString(void) : str(NULL), sz(0)
{
	initForChar('\0');
}

BString::BString(const char c) : str(NULL), sz(0)
{
	initForChar(c);
}

BString::BString(const char *s, int maxlen) : str(NULL), sz(0)
{
  if (s == NULL)
    initForChar('\0');
  else
    init(s, maxlen);
}

BString::BString(const BString& s, int maxlen) : str(NULL), sz(0)
{
	init(s.c_str(), maxlen);
}

BString::~BString(void)
{
	delete [] str;
	sz = 0;
}

void BString::FSadd(const char *s1)
{
  if (s1 == NULL) return;

  int size = length() + strlen(s1) + 1;
	if (size <= capacity())
		strcat(str, s1);
	else {
		char *newS = new char[size];
		strcpy(newS, str);
		strcat(newS, s1);
		delete [] str;
		str = newS;
		sz = size;
	}
}

void BString::reserve(int s)
{
	if (s > sz) {
		sz = s;
		char *newS = new char[s];
		strcpy(newS, str);
		delete [] str;
		str = newS;
	}
}

BString BString::substr(int pos, int count) const
{
   int l = count;

   if (pos + count >= length())
   {
      count = length() - pos;
   }

   BString result(&this->str[pos], count);

   return result;
}

void BString::assign(const BString &s)
{
  int size;

	size = s.length() + 1;
	if (size > sz) {
		sz = size;
		char *newS = new char[size];
		strcpy(newS, s.c_str());
		delete [] str;
		str = newS;
	} else
		strcpy(str, s.c_str());
}

void BString::assign(const char *s)
{
  if (s == NULL) return;

  int size;

	size = strlen(s) + 1;
	if (size > sz) {
		sz = size;
		char *newS = new char[size];
		strcpy(newS, s);
		delete [] str;
		str = newS;
	} else
		strcpy(str, s);
}

int BString::printf(const char *format, ...)
{
  if (format == NULL) return 0;

  int      size, res;
	char     *newS;
	va_list  arg_ptr;

	size = 80;
	do {
		size *= 2;
		if (size > sz) {
			newS = new char[size];
			delete [] str;
			str = newS;
			sz = size;
		}
		va_start(arg_ptr, format);
#ifdef _MSC_VER
		res = _vsnprintf(str, sz, format, arg_ptr);
#endif
#ifdef __GNUC__
		res = vsnprintf(str, sz, format, arg_ptr);
#endif
		va_end(arg_ptr);
		if (res >= 0 && res > sz)
			res = -1;
	} while (res < 0);
	return length();
}

int BString::vsprintf(const char *format, va_list arg_ptr)
{
  if (format == NULL) return 0;

  int     size, res;
	char    *newS;

	size = 80;
	do {
		size *= 2;
		if (size > sz) {
			newS = new char[size];
			delete [] str;
			str = newS;
			sz = size;
		}
#ifdef _MSC_VER
		res = _vsnprintf(str, sz, format, arg_ptr);
#endif
#ifdef __GNUC__
		res = vsnprintf(str, sz, format, arg_ptr);
#endif
		if (res >= 0 && res > sz)
			res = -1;
	} while (res < 0);
	return length();
}

bool BString::operator <  (const BString &s) const
{
	return strcmp(str, s.str)  < 0;
}

bool BString::operator <= (const BString &s) const
{
	return strcmp(str, s.str)  <= 0;
}

bool BString::operator == (const BString &s) const
{
	return strcmp(str, s.str)  == 0;
}

bool BString::operator != (const BString &s) const
{
	return strcmp(str, s.str)  != 0;
}

bool BString::operator >= (const BString &s) const
{
	return strcmp(str, s.str)  >= 0;
}

bool BString::operator >  (const BString &s) const
{
	return strcmp(str, s.str)  > 0;
}

unsigned int BString::length(void) const
{
  return strlen(str);
}


