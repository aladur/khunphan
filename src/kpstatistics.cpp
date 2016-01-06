/*
    kpstatistics.cpp

    Automatic solution finder for KhunPhan game
    Copyright (C) 2001-2016  W. Schwotzer

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
#include "sprinter.h"


KPStatistics::KPStatistics() : playingTime(0), stopWatchActive(false)
{
    Reset();
}

void KPStatistics::Reset()
{
    playingTime     = 0;
    stopWatchActive = false;
    ResetEventCounter(MOVE_COUNTER);
    ResetEventCounter(MOVE_WITH_HELP_CNT);
    ResetEventCounter(USED_CHEATS_CNT);
}

void KPStatistics::Start()
{
    time.ResetRelativeTime();
    stopWatchActive = true;
}

void KPStatistics::Stop()
{
    if (stopWatchActive)
    {
        playingTime     += time.GetRelativeTimeMsl();
        stopWatchActive  = false;
    }
}

unsigned long KPStatistics::GetTotalTime()
{
    if (stopWatchActive)
    {
        return playingTime + time.GetRelativeTimeMsl();
    }
    else
    {
        return playingTime;
    }
}

int KPStatistics::GetEventCounter(tEventCounter i)
{
    if (i < MAX_COUNTER)
    {
        return counter[i];
    }
    return 0;
}

void KPStatistics::SetEventCounter(tEventCounter i, unsigned int n)
{
    if (i < MAX_COUNTER)
    {
        counter[i] = n;
    }
}

void KPStatistics::IncEventCounter(tEventCounter i, unsigned int n /*= 1*/)
{
    if (i < MAX_COUNTER)
    {
        counter[i] += n;
    }
}

void KPStatistics::DecEventCounter(tEventCounter i, unsigned int n /*= 1*/)
{
    if (i < MAX_COUNTER)
    {
        counter[i] -= n;
    }
}

void KPStatistics::ResetEventCounter(tEventCounter i)
{
    if (i < MAX_COUNTER)
    {
        counter[i] = 0;
    }
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
        case RTIME_HH_mm:
            sprinter::sprintf(timeStr, "%02u.%02u",
                              t / 3600000,
                              (t % 3600000) * 100 / 3600000 );
            break;
        case RTIME_HH_mmmm:
            sprinter::sprintf(timeStr, "%02u.%03u",
                              t / 3600000,
                              (t % 3600000) * 10000 / 3600000 );
            break;
        case RTIME_HHMM_ss:
            sprinter::sprintf(timeStr, "%02u:%02u.%02u",
                              t / 3600000,
                              (t % 3600000) / 60000,
                              (t % 60000) * 100 / 60000 );
            break;
        case RTIME_HHMMSS_mmm:
            sprinter::sprintf(timeStr, "%02u:%02u:%02u.%03u",
                              t / 3600000,
                              (t % 3600000) / 60000,
                              (t % 60000)   / 1000,
                              t % 1000                 );
            break;
        case RTIME_HHMMSS:
            sprinter::sprintf(timeStr, "%02u:%02u:%02u",
                              t / 3600000,
                              (t % 3600000) / 60000,
                              (t % 60000)   / 1000      );
            break;
        case RTIME_MM_ss:
            sprinter::sprintf(timeStr, "%02u.%02u",
                              t / 60000,
                              (t % 60000) * 100 / 60000 );
            break;
        case RTIME_msec:
        default:
            sprinter::sprintf(timeStr, "%u", t);
            break;
    }

    return timeStr;
}
