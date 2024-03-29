/*
    bmanualtimer.h

    Automatic solution finder for KhunPhan game
    Copyright (C) 2016-2024  W. Schwotzer

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

#ifndef BMANUALTIMER_H
#define BMANUALTIMER_H


#include "stdafx.h"

class BManualTimer
{
public:
    BManualTimer(QWord MaxTime, bool IsAutoReset,
                 bool IsStarted = false) :
        time(0U), maxTime(MaxTime), isAutoReset(IsAutoReset),
        isStarted(IsStarted)
    {
    }

    inline void Restart()
    {
        isStarted = true;
        time = 0U;
    }

    inline void Reset()
    {
        isStarted = false;
        time = 0U;
    }

    inline bool IsStarted() const
    {
        return isStarted;
    }

    inline bool Add(QWord duration)
    {
        auto isExpired = false;

        if (isStarted)
        {
            time += duration;
            isExpired = (time >= maxTime);

            if (isExpired)
            {
                if (isAutoReset)
                {
                    time -= maxTime;
                }
                else
                {
                    isStarted = false;
                }
            }
        }

        return isExpired;
    }

    inline double GetRelativeTime() const
    {
        return static_cast<double>(time) / maxTime;
    }

    inline QWord GetTime() const
    {
        return time;
    }

private:
    BManualTimer();

    QWord time;
    QWord maxTime;
    bool isAutoReset;
    bool isStarted;
};
#endif
