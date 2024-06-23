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

#ifndef PCP_WIN_DEFINES
#define PCP_WIN_DEFINES

#include <time.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <ws2ipdef.h>
#include <winbase.h> /*GetCurrentProcessId*/ /*link with kernel32.dll*/
#include "stdint.h"
/* windows uses Sleep(miliseconds) method, instead of UNIX sleep(seconds) */
#define sleep(x) Sleep((x) * 1000)

#ifdef _MSC_VER
#define inline __inline /*In Visual Studio inline keyword only available in C++ */
#endif

typedef uint16_t in_port_t;

#if 1 //WINVER<NTDDI_VISTA
static inline const char *pcp_inet_ntop(int af, const void *src, char *dst,
        int cnt)
{
    struct sockaddr_storage srcaddr;
    size_t slen;

    if (af == AF_INET) {
        struct sockaddr_in *sa4=(struct sockaddr_in *)&srcaddr;
        memset(sa4, 0, sizeof(struct sockaddr_in));
        memcpy(&(sa4->sin_addr), src, sizeof(sa4->sin_addr));
        slen=sizeof(struct sockaddr_in);
    } else if (af == AF_INET6) {
        struct sockaddr_in6 *sa6=(struct sockaddr_in6 *)&srcaddr;
        memset(sa6, 0, sizeof(struct sockaddr_in6));
        memcpy(&(sa6->sin6_addr), src, sizeof(sa6->sin6_addr));
        slen=sizeof(struct sockaddr_in6);
    } else {
        return NULL;
    }
    srcaddr.ss_family=af;
    if (WSAAddressToStringA((struct sockaddr *)&srcaddr, (DWORD)slen, 0, dst,
            (LPDWORD) & cnt) != 0) {
        return NULL;
    }
    return dst;
}
#define inet_ntop pcp_inet_ntop
#endif

#define ssize_t SSIZE_T
#define strdup _strdup

#define getpid GetCurrentProcessId

#define snprintf _snprintf

int gettimeofday(struct timeval *tv, struct timezone *tz);

#define MSG_DONTWAIT    0x0

#endif /*PCP_WIN_DEFINES*/
