/*
    kpstatistics.cpp

    Automatic solution finder for KhunPhan game
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


#include "misc1.h"
#include "kpstatistics.h"
#include "btime.h"
#include "fprinter.h"


KPStatistics::KPStatistics() : pTime(NULL), time(0), stopWatchActive(false)
{
  for (unsigned short i = 0; i < MAX_COUNTER; i++)
    ResetEventCounter(i);
}

KPStatistics::~KPStatistics()
{
  delete pTime;
  pTime = NULL;
}

void KPStatistics::Reset()
{
  time            = 0;
  stopWatchActive = false;
  for (unsigned short i = 0; i < MAX_COUNTER; i++)
    ResetEventCounter(i);
}

void KPStatistics::Start()
{
  if (pTime == NULL) pTime = new BTime();

  pTime->ResetRelativeTime();
  stopWatchActive = true;
}

void KPStatistics::Stop()
{
  if (pTime == NULL) pTime = new BTime();

  if (stopWatchActive)
  {
    time            += pTime->GetRelativeTimeMsl();
    stopWatchActive  = false;
  }
}

unsigned long KPStatistics::GetTotalTime()
{
  if (pTime == NULL) pTime = new BTime();

  if (stopWatchActive)
    return time + pTime->GetRelativeTimeMsl();
  else
    return time;
}

int KPStatistics::GetEventCounter(unsigned short i)
{
  if (i < MAX_COUNTER)
    return counter[i];
  return 0;
}

void KPStatistics::SetEventCounter(unsigned short i, unsigned int n)
{
  if (i < MAX_COUNTER)
    counter[i] = n;
}

void KPStatistics::IncEventCounter(unsigned short i, unsigned int n /*= 1*/)
{
  if (i < MAX_COUNTER)
    counter[i] += n;
}

void KPStatistics::DecEventCounter(unsigned short i, unsigned int n /*= 1*/)
{
  if (i < MAX_COUNTER)
    counter[i] -= n;
}

void KPStatistics::ResetEventCounter(unsigned short i)
{
  if (i < MAX_COUNTER)
    counter[i] = 0;
}

std::string KPStatistics::GetTotalTime(tTimeFormat formatID)
{
    return FormatTime(formatID, GetTotalTime());
}

std::string KPStatistics::FormatTime(tTimeFormat formatID, unsigned int t)
{
  std::string timeStr;

  switch (formatID)
  {
    case RTIME_HH_mm:      fprinter::sprintf(timeStr, "%02u.%02u",
                            t / 3600000,
                           (t % 3600000) * 100 / 3600000 ); break;
    case RTIME_HH_mmmm:    fprinter::sprintf(timeStr, "%02u.%03u",
                            t / 3600000,
                           (t % 3600000) * 10000 / 3600000 ); break;
    case RTIME_HHMM_ss:    fprinter::sprintf(timeStr, "%02u:%02u.%02u",
                            t / 3600000,
                           (t % 3600000) / 60000,
                           (t % 60000) * 100 / 60000 ); break;
    case RTIME_HHMMSS_mmm: fprinter::sprintf(timeStr, "%02u:%02u:%02u.%03u",
                            t / 3600000,
                           (t % 3600000) / 60000,
                           (t % 60000)   / 1000,
                            t % 1000                 ); break;
    case RTIME_HHMMSS:     fprinter::sprintf(timeStr, "%02u:%02u:%02u",
                            t / 3600000,
                           (t % 3600000) / 60000,
                           (t % 60000)   / 1000      ); break;
    case RTIME_MM_ss:      fprinter::sprintf(timeStr, "%02u.%02u",
                            t / 60000,
                           (t % 60000) * 100 / 60000 ); break;
    case RTIME_msec:
    default:               fprinter::sprintf(timeStr, "%u", t); break;
  }

  return timeStr;
}
