/*
    blinuxthreadimp.cpp


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

#include "blinuxthreadimp.h"
#include "bthread.h"

typedef  void *(*tThreadProc)(void *);

BLinuxThreadImp::BLinuxThreadImp() : pThreadObj(NULL), finished(false), thread(0)
{
}

BLinuxThreadImp::~BLinuxThreadImp()
{
}

bool BLinuxThreadImp::Start(BThread *aThreadObject)
{
  // pthread_create returns 0 if thread is successfully created
  // the thread identifier is stored in the 'thread'
  pThreadObj = aThreadObject;
  int result = pthread_create(&thread, NULL, (tThreadProc)BLinuxThreadImp::RunImp, this);
  if (result == 0)
    // detach thread to avoid memory leaks
    pthread_detach(thread);
  return (result == 0);
}

bool BLinuxThreadImp::IsFinished()
{
  return finished;
}

void BLinuxThreadImp::Exit(void *retval)
{
  // Attention: this function call will never return!
  finished = true;
  pthread_exit(retval);  
}

// this static function is the thread procedure to
// be called with Start(...)
void *BLinuxThreadImp::RunImp(BLinuxThreadImp *p)
{
  void *result = NULL;

  p->finished = false;
  if (p != NULL && p->pThreadObj != NULL)
    p->pThreadObj->Run();
  p->finished = true;
  return result;
}
