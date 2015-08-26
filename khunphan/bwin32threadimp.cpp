/*
    bwin32threadimp.cpp


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

#ifdef _POSIX_
#undef _POSIX_
#endif

#include <process.h>
#include "bwin32threadimp.h"
#include "bthread.h"

#ifndef THREAD_STACK_SIZE 
#define THREAD_STACK_SIZE 16384
#endif

typedef  void ( __cdecl *tThreadProc )( void * );

BWin32ThreadImp::BWin32ThreadImp() : pThreadObj(NULL), finished(false), thread(-1)
{
}

BWin32ThreadImp::~BWin32ThreadImp()
{
}

bool BWin32ThreadImp::Start(BThread *aThreadObject)
{
  unsigned stackSize = THREAD_STACK_SIZE;

  pThreadObj = aThreadObject;
  unsigned long thread = _beginthread((tThreadProc)BWin32ThreadImp::RunImp, stackSize, this);
  // return true if thread successfully has been created
  return (thread != -1);
}

bool BWin32ThreadImp::IsFinished()
{
  return finished;
}

void BWin32ThreadImp::Exit(void *)
{
  // Attention: this function call will never return!
  finished = true;
  _endthread();  
}

// this static function is the thread procedure to
// be called with Start(...)
void BWin32ThreadImp::RunImp(BWin32ThreadImp *p)
{
  p->finished = false;
  if (p != NULL && p->pThreadObj != NULL)
    p->pThreadObj->Run();
  p->finished = true;
  p->thread = -1;
}
