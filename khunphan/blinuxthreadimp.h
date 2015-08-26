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

#ifndef BLINUXTHREADIMP_H
#define BLINUXTHREADIMP_H

#include <pthread.h>
#include "bthreadimp.h"

class BThread;

class BLinuxThreadImp : public BThreadImp
{
public:
	BLinuxThreadImp();
	virtual ~BLinuxThreadImp();
  bool Start(BThread *pThread);
  bool IsFinished();
  void Exit(void *retval = NULL);
private:
  static void *RunImp(BLinuxThreadImp *p);
  BThread *pThreadObj;
  bool finished;
  pthread_t thread;
};

#endif
