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

#include <stdio.h>
#include <string.h>
#include <assert.h>
#ifdef WIN32
#include "windows/pcp_win_defines.h"
#else  //WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#ifndef PCP_SOCKET_IS_VOIDPTR
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#endif //PCP_SOCKET_IS_VOIDPTR
#endif //!WIN32
#include "pcp.h"
#include "unp.h"
#include "pcp_utils.h"
#include "pcp_socket.h"

static PCP_SOCKET pcp_socket_create_impl(int domain, int type, int protocol);
static int pcp_socket_recvfrom_impl(PCP_SOCKET sock, void *buf, size_t len,
        int flags, struct sockaddr *src_addr, socklen_t *addrlen);
static int pcp_socket_sendto_impl(PCP_SOCKET sock, const void *buf,
        size_t len, int flags, struct sockaddr *dest_addr, socklen_t addrlen);
static int pcp_socket_close_impl(PCP_SOCKET sock);

pcp_socket_vt_t default_socket_vt={
        pcp_socket_create_impl,
        pcp_socket_recvfrom_impl,
        pcp_socket_sendto_impl,
        pcp_socket_close_impl
};

#ifdef WIN32
// function calling WSAStartup (used in pcp-server and pcp_app)
int pcp_win_sock_startup()
{
    int err;
    WORD wVersionRequested;
    WSADATA wsaData;

    /* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
    wVersionRequested=MAKEWORD(2, 2);
    err=WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        /* Tell the user that we could not find a usable */
        /* Winsock DLL.                                  */
        perror("WSAStartup failed with error");
        return 1;
    }

    return 0;
}

/* function calling WSACleanup
 *  returns 0 on success and 1 on failure
 */
int pcp_win_sock_cleanup()
{
    if (WSACleanup() == PCP_SOCKET_ERROR) {
        printf("WSACleanup failed.\n");
        return 1;
    }
    return 0;
}
#endif

void pcp_fill_in6_addr(struct in6_addr *dst_ip6, uint16_t *dst_port,
        struct sockaddr *src)
{
    if (src->sa_family == AF_INET) {
        struct sockaddr_in *src_ip4=(struct sockaddr_in *)src;

        if (dst_ip6) {
            if (src_ip4->sin_addr.s_addr != INADDR_ANY) {
                S6_ADDR32(dst_ip6)[0]=0;
                S6_ADDR32(dst_ip6)[1]=0;
                S6_ADDR32(dst_ip6)[2]=htonl(0xFFFF);
                S6_ADDR32(dst_ip6)[3]=src_ip4->sin_addr.s_addr;
            } else {
                unsigned i;
                for (i=0; i < 4; ++i)
                    S6_ADDR32(dst_ip6)[i]=0;
            }
        }
        if (dst_port) {
            *dst_port=src_ip4->sin_port;
        }
    } else if (src->sa_family == AF_INET6) {
        struct sockaddr_in6 *src_ip6=(struct sockaddr_in6 *)src;

        if (dst_ip6) {
            memcpy(dst_ip6, src_ip6->sin6_addr.s6_addr, sizeof(*dst_ip6));
        }
        if (dst_port) {
            *dst_port=src_ip6->sin6_port;
        }
    }
}

void pcp_fill_sockaddr(struct sockaddr *dst, struct in6_addr *sip,
        uint16_t sport, int ret_ipv6_mapped_ipv4, uint32_t scope_id)
{
    if ((!ret_ipv6_mapped_ipv4) && (IN6_IS_ADDR_V4MAPPED(sip))) {
        struct sockaddr_in *s=(struct sockaddr_in *)dst;

        s->sin_family=AF_INET;
        s->sin_addr.s_addr=S6_ADDR32(sip)[3];
        s->sin_port=sport;
        SET_SA_LEN(s, sizeof(struct sockaddr_in));
    } else {
        struct sockaddr_in6 *s=(struct sockaddr_in6 *)dst;

        s->sin6_family=AF_INET6;
        s->sin6_addr=*sip;
        s->sin6_port=sport;
        s->sin6_scope_id=scope_id;
        SET_SA_LEN(s, sizeof(struct sockaddr_in6));
    }
}

#ifndef PCP_SOCKET_IS_VOIDPTR
static pcp_errno pcp_get_error()
{
#ifdef WIN32
    int errnum=WSAGetLastError();

    switch (errnum) {
        case WSAEADDRINUSE:
            return PCP_ERR_ADDRINUSE;
        case WSAEWOULDBLOCK:
            return PCP_ERR_WOULDBLOCK;
        default:
            return PCP_ERR_UNKNOWN;
    }
#else
    switch (errno) {
        case EADDRINUSE:
            return PCP_ERR_ADDRINUSE;
//        case EAGAIN:
        case EWOULDBLOCK:
            return PCP_ERR_WOULDBLOCK;
        default:
            return PCP_ERR_UNKNOWN;
    }
#endif
}
#endif

PCP_SOCKET pcp_socket_create(struct pcp_ctx_s *ctx, int domain, int type,
        int protocol)
{
    assert(ctx && ctx->virt_socket_tb && ctx->virt_socket_tb->sock_create);

    return ctx->virt_socket_tb->sock_create(domain, type, protocol);
}

ssize_t pcp_socket_recvfrom(struct pcp_ctx_s *ctx, void *buf, size_t len,
        int flags, struct sockaddr *src_addr, socklen_t *addrlen)
{
    assert(ctx && ctx->virt_socket_tb && ctx->virt_socket_tb->sock_recvfrom);

    return ctx->virt_socket_tb->sock_recvfrom(ctx->socket, buf, len, flags,
            src_addr, addrlen);
}

ssize_t pcp_socket_sendto(struct pcp_ctx_s *ctx, const void *buf, size_t len,
        int flags, struct sockaddr *dest_addr, socklen_t addrlen)
{
    assert(ctx && ctx->virt_socket_tb && ctx->virt_socket_tb->sock_sendto);

    return ctx->virt_socket_tb->sock_sendto(ctx->socket, buf, len, flags,
            dest_addr, addrlen);
}

int pcp_socket_close(struct pcp_ctx_s *ctx)
{
    assert(ctx && ctx->virt_socket_tb && ctx->virt_socket_tb->sock_close);

    return ctx->virt_socket_tb->sock_close(ctx->socket);
}

static PCP_SOCKET pcp_socket_create_impl(int domain, int type, int protocol)
{
#ifdef PCP_SOCKET_IS_VOIDPTR
    return PCP_INVALID_SOCKET;
#else
    PCP_SOCKET s;
    uint32_t flg;
    unsigned long iMode=1;
    struct sockaddr_storage sas;
    struct sockaddr_in *sin=(struct sockaddr_in *)&sas;
    struct sockaddr_in6 *sin6=(struct sockaddr_in6 *)&sas;

    OSDEP(iMode);
    OSDEP(flg);

    memset(&sas, 0, sizeof(sas));
    sas.ss_family=(ADDRESS_FAMILY)domain;
    if (domain == AF_INET) {
        sin->sin_port=htons(5350);
        SET_SA_LEN(sin, sizeof(struct sockaddr_in));
    } else if (domain == AF_INET6) {
        sin6->sin6_port=htons(5350);
        SET_SA_LEN(sin6, sizeof(struct sockaddr_in6));
    } else {
        PCP_LOG(PCP_LOGLVL_ERR, "Unsupported socket domain:%d", domain);
    }

    s=(PCP_SOCKET)socket(domain, type, protocol);
    if (s == PCP_INVALID_SOCKET)
        return PCP_INVALID_SOCKET;

#ifdef WIN32
    if (ioctlsocket(s, FIONBIO, &iMode)) {
        PCP_LOG(PCP_LOGLVL_ERR, "%s",
                "Unable to set nonblocking mode for socket.");
        CLOSE(s);
        return PCP_INVALID_SOCKET;
    }
#else
    flg=fcntl(s, F_GETFL, 0);
    if (fcntl(s, F_SETFL, flg | O_NONBLOCK)) {
        PCP_LOG(PCP_LOGLVL_ERR, "%s",
                "Unable to set nonblocking mode for socket.");
        CLOSE(s);
        return PCP_INVALID_SOCKET;
    }
#endif
#ifdef PCP_USE_IPV6_SOCKET
    flg=0;
    if (PCP_SOCKET_ERROR
            == setsockopt(s, IPPROTO_IPV6, IPV6_V6ONLY, (char *)&flg,
                    sizeof(flg))) {
        PCP_LOG(PCP_LOGLVL_ERR, "%s",
                "Dual-stack sockets are not supported on this platform. "
                "Recompile library with disabled IPv6 support.");
        CLOSE(s);
        return PCP_INVALID_SOCKET;
    }
#endif //PCP_USE_IPV6_SOCKET
    while (bind(s, (struct sockaddr *)&sas,
            (int)SA_LEN((struct sockaddr *)&sas)) == PCP_SOCKET_ERROR) {
        if (pcp_get_error() == PCP_ERR_ADDRINUSE) {
            if (sas.ss_family == AF_INET) {
                sin->sin_port=htons(ntohs(sin->sin_port) + 1);
            } else {
                sin6->sin6_port=htons(ntohs(sin6->sin6_port) + 1);
            }
        } else {
            PCP_LOG(PCP_LOGLVL_ERR, "%s", "bind error");
            CLOSE(s);
            return PCP_INVALID_SOCKET;
        }
    }
    PCP_LOG(PCP_LOGLVL_DEBUG, "%s: return %d", __FUNCTION__, s);
    return s;
#endif
}

static int pcp_socket_recvfrom_impl(PCP_SOCKET sock, void *buf,
        size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen)
{
    int ret=-1;

#ifndef PCP_SOCKET_IS_VOIDPTR
    ret=recvfrom(sock, (char*)buf, (int)len, flags, src_addr, addrlen);
    if (ret == PCP_SOCKET_ERROR) {
        if (pcp_get_error() == PCP_ERR_WOULDBLOCK) {
            ret=PCP_ERR_WOULDBLOCK;
        } else {
            ret=PCP_ERR_RECV_FAILED;
        }
    }
#endif  //PCP_SOCKET_IS_VOIDPTR

    return ret;
}

static int pcp_socket_sendto_impl(PCP_SOCKET sock, const void *buf,
    size_t len, int flags UNUSED, struct sockaddr *dest_addr, socklen_t addrlen)
{
        int ret=-1;

#ifndef PCP_SOCKET_IS_VOIDPTR
    ret=sendto(sock, (const char*)buf, (int)len, 0, dest_addr, addrlen);
    if ((ret == PCP_SOCKET_ERROR) || (ret != (ssize_t)len)) {
        if (pcp_get_error() == PCP_ERR_WOULDBLOCK) {
            ret=PCP_ERR_WOULDBLOCK;
        } else {
            ret=PCP_ERR_SEND_FAILED;
        }
    }
#endif
    return ret;
}

static int pcp_socket_close_impl(PCP_SOCKET sock)
{
#ifndef PCP_SOCKET_IS_VOIDPTR
    return CLOSE(sock);
#else
    return PCP_SOCKET_ERROR;
#endif
}
