/*
    bstring.h


    Basic class containing a string definition
    Copyright (C) 1999-2015  W. Schwotzer

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

#ifndef __bstring_h__
#define __bstring_h__

#include "misc1.h"
#ifdef HAVE_STDARG_H
  #include <stdarg.h>  // needed for va_list
#endif

class BString {

private:
	char    *str;   // pointer to char array
	int             sz;             // allocated size

protected:
	void init(const char *s, int maxlen);
	void initForChar(const char c);
	void FSadd(const char *s1);

public:
	BString();
	BString(const BString& s, int maxlen = -1);
	BString(const char *s, int maxlen = -1);
	BString(const char c);
	virtual ~BString();

	unsigned int length(void) const;
	bool          empty(void) const;
	const char   *c_str(void) const;
	const char   *data(void) const;
	int          capacity(void) const;

	void assign(const BString &s);
	void assign(const char *s);
	void reserve(int s);
        BString substr(int pos, int count) const;

	char& operator [] (int pos);

	BString& operator = (const BString& s);
	BString& operator = (const char *s);
	BString& operator = (const char c);

	BString& operator += (const BString& s);
	BString& operator += (const char *s);
	BString& operator += (const char c);

	BString operator + (const BString& s) const;
	BString operator + (const char *s) const;
	BString operator + (const char c) const;

	bool operator <  (const BString &s) const;
	bool operator <= (const BString &s) const;
	bool operator == (const BString &s) const;
	bool operator != (const BString &s) const;
	bool operator >= (const BString &s) const;
	bool operator >  (const BString &s) const;

	int printf(const char *format, ...);
	int vsprintf(const char *format, va_list arg_ptr);
};

inline bool BString::empty(void) const { return str[0] == '\0'; }
inline const char *BString::c_str(void) const { return &str[0]; }
inline const char *BString::data(void) const { return &str[0]; }
inline int BString::capacity(void) const { return sz; }

inline char& BString::operator [] (int pos) { return str[pos]; }

inline BString& BString::operator = (const BString& s) { assign(s); return *this; }
inline BString& BString::operator = (const char *_s) { assign(_s); return *this; }
inline BString& BString::operator = (const char c) { char _s[2]; _s[0] = c; _s[1] = '\0'; assign(_s); return *this; }

inline BString& BString::operator += (const BString& s) { FSadd(s.c_str()); return *this; }
inline BString& BString::operator += (const char *s) { FSadd(s); return *this; }
inline BString& BString::operator += (const char c) { char s[2]; s[0] = c; s[1] = '\0'; FSadd(s); return *this; }

inline BString BString::operator + (const BString& s) const  { BString r(this->c_str()); r.FSadd(s.c_str()); return r; }
inline BString BString::operator + (const char *s) const { BString r(this->c_str()); r.FSadd(s); return r; }
inline BString BString::operator + (const char c) const { char s[2]; s[0] = c; s[1] = '\0'; BString r(this->c_str()); r.FSadd(s); return r; }

#endif // #ifndef __bstring_h__

