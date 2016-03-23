/*
    kpstatistics.h

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

#ifndef KPSTATISTICS_H
#define KPSTATISTICS_H

#include <string>
#include <vector>
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

enum tTimeFormat
{
    RTIME_msec       = 0,    // Format:   +mmm
    RTIME_HH_mm      = 321,  // Format:   +HH.mm
    RTIME_HH_mmmm    = 322,  // Format:   +HH.mmmm
    RTIME_HHMM_ss    = 323,  // Format:   +HH:MM.ss
    RTIME_HHMMSS_mmm = 324,  // Format:   +HH:MM:SS.mmm
    RTIME_HHMMSS     = 325,  // Format:   +HH:MM:SS
    RTIME_MM_ss      = 326   // Format:   +MM.ss
};


class KPStatistics
{
public:
    KPStatistics();

    void Reset(); // Stop capturing time, reset all statistic values

    void Start();                 // Start capturing time
    void Stop();                  // Stop capturing time
    void SetPlayingTime(unsigned long t)
    {
        playingTime = t;
    };
    unsigned long GetTotalTime();              // Returns the total elapsed time
                                               // in milliseconds
    std::string GetTotalTime(tTimeFormat       // Return a formatted time string
                             formatID);
    void SetEventCounter(tEventCounter type,   // sets the current value of
                         unsigned int n);      // event counter to n
    int  GetEventCounter(tEventCounter type);  // returns the current value of
                                               // event counter
    void IncEventCounter(tEventCounter type);  // increment event counter by 1
    void DecEventCounter(tEventCounter type);  // decrement event counter by 1
    void ResetEventCounter(tEventCounter type);// Reset event counter
protected:
    std::string FormatTime(tTimeFormat formatID, unsigned int t);

private:
    BTime time;
    std::vector<unsigned int> counter;
    unsigned long  playingTime;
    bool stopWatchActive;
};
#endif
