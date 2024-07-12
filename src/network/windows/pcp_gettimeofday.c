/*
 Copyright (c) 2014 by Cisco Systems, Inc.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#include "default_config.h"
#endif

#ifndef HAVE_GETTIMEOFDAY
#include <windows.h>
#include <time.h>
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else /* defined(_MSC_VER) || defined(_MSC_EXTENSIONS)*/
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif /* defined(_MSC_VER) || defined(_MSC_EXTENSIONS)*/

/* custom implementation of the gettimeofday function
 for Windows platform. */

struct timezone {
    int tz_minuteswest; /* minutes W of Greenwich */
    int tz_dsttime; /* type of dst correction */
};

int gettimeofday(struct timeval *tv, struct timezone *tz)
{
    FILETIME ft;
    unsigned __int64 tmpres=0;
    static int tzflag=0;
    long tz_seconds=0;
    int tz_daylight=0;

    if (NULL != tv) {
        GetSystemTimeAsFileTime(&ft);

        tmpres|=ft.dwHighDateTime;
        tmpres<<=32;
        tmpres|=ft.dwLowDateTime;

        tmpres/=10; /*convert into microseconds*/
        /*converting file time to unix epoch*/
        tmpres-=DELTA_EPOCH_IN_MICROSECS;
        tv->tv_sec=(long)(tmpres / 1000000UL);
        tv->tv_usec=(long)(tmpres % 1000000UL);
    }

    if (tz) {
        if (!tzflag) {
            _tzset();
            tzflag++;
        }
        if (_get_timezone(&tz_seconds)) {
            return -1;
        }
        if (_get_daylight(&tz_daylight)) {
            return -1;
        }
        tz->tz_minuteswest=tz_seconds / 60;
        tz->tz_dsttime=tz_daylight;
    }

    return 0;
}

#endif //HAVE_GETTIMEOFDAY
