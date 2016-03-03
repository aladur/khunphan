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


#include "stdafx.h"
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
    counter.push_back(0);
    counter.push_back(0);
//    counter.push_back(0);
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

int KPStatistics::GetEventCounter(tEventCounter type)
{
    return counter.at(type);
}

void KPStatistics::SetEventCounter(tEventCounter type, unsigned int n)
{
    counter.at(type) = n;
}

void KPStatistics::IncEventCounter(tEventCounter type)
{
    counter.at(type)++;
}

void KPStatistics::DecEventCounter(tEventCounter type)
{
    counter.at(type)--;
}

void KPStatistics::ResetEventCounter(tEventCounter type)
{
    counter.at(type) = 0;
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
