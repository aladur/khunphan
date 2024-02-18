/*
    kpstatistics.h

    Automatic solution finder for KhunPhan game
    Copyright (C) 2001-2024  W. Schwotzer

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

#ifndef KPSTATISTICS_H
#define KPSTATISTICS_H

#include <string>
#include <map>
#include "btime.h"

// Time display formats:
// +HH:  two (or more) digit hours
// MM:   two digit minutes (0..59)
// +MM:  two (or more) digit minutes
// SS:   two digit seconds (0..59)
// mm:   decimal minutes (0..99)
// mmmm: decimal minutes (0..9900)
// +mmm: three (or more) digit milliseconds
// ss:   decimal seconds (0..99)
// mmm:  decimal milliseconds (0..999)

enum class TimeFormat
{
    msec,       // Format: +mmm
    HH_mm,      // Format: +HH.mm
    HH_mmmm,    // Format: +HH.mmmm
    HHMM_ss,    // Format: +HH:MM.ss
    HHMMSS_mmm, // Format: +HH:MM:SS.mmm
    HHMMSS,     // Format: +HH:MM:SS
    MM_ss       // Format: +MM.ss
};

class KPStatistics
{
public:
    KPStatistics();

    // Stop capturing time, reset all statistic values
    void Reset();
    // Start capturing play time
    void Start();
    // Stop capturing play time
    void Stop();
    void SetPlayingTime(QWord t)
    {
        playingTime = t;
    };

    // Returns the total elapsed time in milliseconds
    QWord GetTotalTime();
    // Return a formatted time string
    std::string GetTotalTime(TimeFormat formatID);

    // Set the value of event counter type to n
    void Set(EventCounter type, unsigned int n);
    // Returns the current value of event counter type
    int  Get(EventCounter type);
    // Increment event counter type by 1
    void Increment(EventCounter type);
    // Decrement event counter type by 1
    void Decrement(EventCounter type);
    // Reset event counter type
    void Reset(EventCounter type);

    static std::string FormatTime(TimeFormat formatID, QWord t);

private:
    BTime time;
    std::map<EventCounter, unsigned int> counter;
    QWord playingTime;
    bool stopWatchActive;
};
#endif
