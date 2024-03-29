/*
    btime.cpp


    Basic class for platform independent high resolution time support
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

#include "stdafx.h"
#include "btime.h"

// This class could also be realized as a envelope/letter pattern
// but this is still the most efficient C++ implementation

BTime::BTime() : lapTime(0)
{
}

void BTime::ResetRelativeTime()
{
    lapTime = GetTimeUsll();
}

QWord BTime::GetRelativeTimeMsll(bool reset /*= false*/)
{
    return GetRelativeTimeUsll(reset) / 1000U;
}

QWord BTime::GetTimeMsll()
{
    return GetTimeUsll() / 1000U;
}


#ifdef __linux__
QWord BTime::GetRelativeTimeUsll(bool reset /*= false*/)
{
    auto currentTime = GetTimeUsll();
    auto result = currentTime - lapTime;

    if (reset)
    {
        lapTime = currentTime;
    }

    return result;
}

double BTime::GetRelativeTimeUsf(bool  reset /*= false*/)
{
    auto currentTime = GetTimeUsll();
    auto result = static_cast<double>(currentTime - lapTime);

    if (reset)
    {
        lapTime = currentTime;
    }

    return result;
}

// return time in us as a unsigned int 64 Bit value
QWord BTime::GetTimeUsll()
{
    struct timeval tv;

    gettimeofday(&tv, nullptr);
    return (static_cast<QWord>(tv.tv_sec) * 1000000 + tv.tv_usec);
}

// return time in us as a double value
double BTime::GetTimeUsf()
{
    struct timeval tv;

    gettimeofday(&tv, nullptr);
    return (static_cast<double>(tv.tv_sec) * 1000000.0 + tv.tv_usec);
}
#else
#ifdef _WIN32
QWord BTime::GetRelativeTimeUsll(bool reset /*= false*/)
{
    auto currentTime = GetTimeUsll();
    auto result = currentTime - lapTime;

    if (reset)
    {
        lapTime = currentTime;
    }

    return result;
}

double BTime::GetRelativeTimeUsf(bool  reset /*= false*/)
{
    auto currentTime = GetTimeUsll();
    auto result = (double)(SQWord)(currentTime - lapTime);

    if (reset)
    {
        lapTime = currentTime;
    }

    return result;
}

// return time in us as a unsigned int 64 Bit value
QWord BTime::GetTimeUsll()
{
    LARGE_INTEGER count, freq;

    if (QueryPerformanceCounter(&count))
    {
        QueryPerformanceFrequency(&freq);
        return static_cast<QWord>(count.QuadPart) * 1000000 /
               freq.QuadPart;
    }
    else
    {
        return static_cast<QWord>(0);
    }

}

// return time in us as a double value
double BTime::GetTimeUsf()
{
    LARGE_INTEGER count, freq;

    if (QueryPerformanceCounter(&count))
    {
        QueryPerformanceFrequency(&freq);
        return static_cast<double>(count.QuadPart) * 1000000 /
               freq.QuadPart;
    }
    else
    {
        return 0.0;
    }

}
#else
#error "Unsupported platform"
#endif
#endif
