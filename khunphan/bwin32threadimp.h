/*
    blinuxthreadimp.h


    Automatic solution finder for KhunPhan game
    Copyright (C) 2001,2002,2003  W. Schwotzer

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

#ifndef _BWIN32THREADIMP_H_
#define _BWIN32THREADIMP_H_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "bthreadimp.h"


class BThread;

class BWin32ThreadImp : public BThreadImp
{
public:
	BWin32ThreadImp();
	virtual ~BWin32ThreadImp();
  bool Start(BThread *pThread);
  bool IsFinished();
  void Exit(void *retval = NULL);
private:
  static void RunImp(BWin32ThreadImp *p);
  BThread *pThreadObj;
  bool finished;
  unsigned long thread;
};

#endif
