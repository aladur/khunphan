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
#include <iomanip>
#include <array>
#include "kpstatistics.h"


KPStatistics::KPStatistics() : playingTime(0), stopWatchActive(false)
{
    Reset();
}

void KPStatistics::Reset()
{
    std::array<EventCounter, 3> all_events
    {
        EventCounter::Moves,
        EventCounter::MovesWithHint,
        EventCounter::MovesWithCheat
    };

    playingTime     = 0;
    stopWatchActive = false;
    counter.clear();

    for (auto event : all_events)
    {
        counter.insert({event, 0});
    }
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

int KPStatistics::Get(EventCounter type)
{
    return counter.at(type);
}

void KPStatistics::Set(EventCounter type, unsigned int n)
{
    counter.at(type) = n;
}

void KPStatistics::Increment(EventCounter type)
{
    counter.at(type)++;
}

void KPStatistics::Decrement(EventCounter type)
{
    counter.at(type)--;
}

void KPStatistics::Reset(EventCounter type)
{
    counter.at(type) = 0;
}

std::string KPStatistics::GetTotalTime(TimeFormat formatID)
{
    return FormatTime(formatID, GetTotalTime());
}

std::string KPStatistics::FormatTime(TimeFormat formatID, unsigned int t)
{
    std::stringstream timeStr;
    using std::setw;
    using std::setfill;

    switch (formatID)
    {
        case TimeFormat::HH_mm:
            timeStr << setw(2) << setfill('0') << t / 3600000 << "."
                    << setw(2) << setfill('0') << (t % 3600000) * 100 / 3600000;
            break;

        case TimeFormat::HH_mmmm:
            timeStr << setw(2) << setfill('0') << t / 3600000 << "."
                    << setw(3) << setfill('0')
                    << (t % 3600000) * 10000 / 3600000 << ".";
            break;

        case TimeFormat::HHMM_ss:
            timeStr << setw(2) << setfill('0') << t / 3600000 << ":"
                    << setw(2) << setfill('0') << (t % 3600000) / 60000 << "."
                    << setw(2) << setfill('0') << (t % 60000) * 100 / 60000;
            break;

        case TimeFormat::HHMMSS_mmm:
            timeStr << setw(2) << setfill('0') << t / 3600000 << ":"
                    << setw(2) << setfill('0') << (t % 3600000) / 60000 << ":"
                    << setw(2) << setfill('0') << (t % 60000) / 1000 << "."
                    << setw(3) << setfill('0') << t % 1000;
            break;

        case TimeFormat::HHMMSS:
            timeStr << setw(2) << setfill('0') << t / 3600000 << ":"
                    << setw(2) << setfill('0') << (t % 3600000) / 60000 << ":"
                    << setw(2) << setfill('0') << (t % 60000) / 1000;
            break;

        case TimeFormat::MM_ss:
            timeStr << setw(2) << setfill('0') << t / 60000 << "."
                    << setw(2) << setfill('0') << (t % 60000) * 100 / 60000;

            break;

        case TimeFormat::msec:
        default:
            timeStr << t;
            break;
    }

    return timeStr.str();
}
