/*
    btime.cpp

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

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#ifdef LINUX
#include <sys/time.h>
#endif
#include "misc1.h"
#include "btime.h"

// This class could also be realized as a envelope/letter pattern
// but this is still the most efficient C++ implementation

BTime::BTime() : lapTime(0)
{
}

BTime::~BTime()
{
}

void BTime::ResetRelativeTime()
{
  lapTime = GetTimeUsll();
}

unsigned long BTime::GetRelativeTimeMsl(bool reset /*= false*/)
{
  return (unsigned long)(GetRelativeTimeUsll(reset) / 1000);
}

unsigned long BTime::GetTimeMsl()
{
  return (unsigned long)(GetTimeUsll() / 1000);
}


#ifdef LINUX
uint64_t BTime::GetRelativeTimeUsll(bool reset /*= false*/)
{
  uint64_t currentTime = GetTimeUsll();
  uint64_t result = currentTime - lapTime;
  if (reset) lapTime = currentTime;

  return result;
}

double BTime::GetRelativeTimeUsf(bool  reset /*= false*/)
{
  uint64_t currentTime = GetTimeUsll();
  double result = (double)(currentTime - lapTime);
  if (reset) lapTime = currentTime;

  return result;
}

// return time in us as a unsigned int 64 Bit value
uint64_t BTime::GetTimeUsll()
{
  struct timeval tv;

  gettimeofday(&tv, NULL);
  return ((uint64_t)tv.tv_sec * 1000000 + tv.tv_usec);
}

// return time in us as a double value
double BTime::GetTimeUsf()
{
  struct timeval tv;

  gettimeofday(&tv, NULL);
  return ((double)tv.tv_sec * 1000000.0 + tv.tv_usec);
}
#endif

#ifdef WIN32
uint64_t BTime::GetRelativeTimeUsll(bool reset /*= false*/)
{
  uint64_t currentTime = GetTimeUsll();
  uint64_t result = currentTime - lapTime;
  if (reset) lapTime = currentTime;

  return result;
}

double BTime::GetRelativeTimeUsf(bool  reset /*= false*/)
{
  uint64_t currentTime = GetTimeUsll();
  double result = (double)(int64_t)(currentTime - lapTime);
  if (reset) lapTime = currentTime;

  return result;
}

// return time in us as a unsigned int 64 Bit value
uint64_t BTime::GetTimeUsll()
{
  LARGE_INTEGER count, freq;

  if (QueryPerformanceCounter(&count)) {
    QueryPerformanceFrequency(&freq);
    return (uint64_t)count.QuadPart * 1000000 / (uint64_t)freq.QuadPart;
  } else
    return 0;

}

// return time in us as a double value
double BTime::GetTimeUsf()
{
  LARGE_INTEGER count, freq;

  if (QueryPerformanceCounter(&count)) {
    QueryPerformanceFrequency(&freq);
    return (double)count.QuadPart * 1000000 / (double)freq.QuadPart;
  } else
    return 0.0;

}
#endif
