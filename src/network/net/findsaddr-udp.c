/*-
 * Copyright (c) 2010 Bjoern A. Zeeb <bz@FreeBSD.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#include "default_config.h"
#endif

#include <string.h>
#ifndef WIN32
# include <sys/types.h>
# include <sys/socket.h>
#include <netinet/in.h>
#else
# include <winsock2.h>
# include <ws2tcpip.h> /*sockaddr, addrinfo etc.*/
# include <stdint.h>
#endif /*WIN32*/

#include "pcp.h"
#include "findsaddr.h"
#include "unp.h"

/*
 * Return the source address for the given destination address.
 *
 * This makes use of proper source address selection in the FreeBSD kernel
 * even taking jails into account (sys/netinet/in_pcb.c:in_pcbladdr()).
 * We open a UDP socket, and connect to the destination, letting the kernel
 * do the bind and then read the source IPv4 address using getsockname(2).
 * This has multiple advantages: no need to do PF_ROUTE operations possibly
 * needing special privileges, jails properly taken into account and most
 * important - getting the result the kernel would give us rather than
 * best-guessing ourselves.
 */

#ifndef SOCKET
#define SOCKET int
#endif

#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif

const char *findsaddr(const struct sockaddr_in *to,
        struct in6_addr *from)
{
    const char *errstr;
    struct sockaddr_in cto, cfrom;

    socklen_t len;

    auto s=socket(AF_INET, SOCK_DGRAM, 0);
    if (s == (unsigned long long)(INVALID_SOCKET))
        return ("failed to open DGRAM socket for src addr selection.");
    errstr=NULL;
    len=sizeof(struct sockaddr_in);
    memcpy(&cto, to, len);
    cto.sin_port=htons(65535); /* Dummy port for connect(2). */
    if (connect(s, (struct sockaddr *)&cto, len) == -1) {
        errstr="failed to connect to peer for src addr selection.";
        goto err;
    }

    if (getsockname(s, (struct sockaddr *)&cfrom, &len) == -1) {
        errstr="failed to get socket name for src addr selection.";
        goto err;
    }

    if (len != sizeof(struct sockaddr_in) || cfrom.sin_family != AF_INET) {
        errstr="unexpected address family in src addr selection.";
        goto err;
    }

    ((uint32_t *)from)[0]=0;
    ((uint32_t *)from)[1]=0;
    ((uint32_t *)from)[2]=htonl(0xffff);
    ((uint32_t *)from)[3]=cfrom.sin_addr.s_addr;

err:
    (void)CLOSE(s);

    /* No error (string) to return. */
    return (errstr);
}

const char *findsaddr6(const struct sockaddr_in6 *to,
        struct in6_addr *from)
{
    const char *errstr;
    struct sockaddr_in6 cto, cfrom;
    socklen_t len;
    uint32_t sock_flg=0;

    if (IN6_IS_ADDR_LOOPBACK(&to->sin6_addr)) {
        memcpy(from, &to->sin6_addr, sizeof(struct in6_addr));
        return NULL;
    }

    auto s=socket(AF_INET6, SOCK_DGRAM, 0);
    if (s == (unsigned long long)(INVALID_SOCKET))
        return ("failed to open DGRAM socket for src addr selection.");

    errstr=NULL;

    //Enable Dual-stack socket for Vista and higher
    if (setsockopt(s, IPPROTO_IPV6, IPV6_V6ONLY, (char *)&sock_flg,
            sizeof(sock_flg)) == -1) {
        errstr="setsockopt failed to set dual stack mode.";
        goto err;
    }

    len=sizeof(struct sockaddr_in6);
    memcpy(&cto, to, len);
    cto.sin6_port=htons(65535); /* Dummy port for connect(2). */
    if (connect(s, (struct sockaddr *)&cto, len) == -1) {
        errstr="failed to connect to peer for src addr selection.";
        goto err;
    }

    if (getsockname(s, (struct sockaddr *)&cfrom, &len) == -1) {
        errstr="failed to get socket name for src addr selection.";
        goto err;
    }

    if (len != sizeof(struct sockaddr_in6) || cfrom.sin6_family != AF_INET6) {
        errstr="unexpected address family in src addr selection.";
        goto err;
    }

    memcpy(from->s6_addr, cfrom.sin6_addr.s6_addr, sizeof(struct in6_addr));

err:
    (void) CLOSE(s);

    /* No error (string) to return. */
    return (errstr);
}

/* end */
