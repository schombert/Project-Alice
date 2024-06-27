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

#ifndef PCP_SOCKET_H
#define PCP_SOCKET_H

#include "pcp.h"

#ifdef PCP_SOCKET_IS_VOIDPTR
#define PD_SOCKET_STARTUP()
#define PD_SOCKET_CLEANUP()
#define PCP_INVALID_SOCKET NULL
#define PCP_SOCKET_ERROR (-1)

#ifdef WIN32
#define CLOSE(sockfd) closesocket(sockfd)
#else
#define CLOSE(sockfd) close(sockfd)
#endif

#else

#ifdef WIN32

int pcp_win_sock_startup();
int pcp_win_sock_cleanup();

#define PD_SOCKET_STARTUP pcp_win_sock_startup
#define PD_SOCKET_CLEANUP pcp_win_sock_cleanup
#define PCP_SOCKET_ERROR SOCKET_ERROR
#define PCP_INVALID_SOCKET INVALID_SOCKET
#define CLOSE(sockfd) closesocket(sockfd)

#else

#define PD_SOCKET_STARTUP()
#define PD_SOCKET_CLEANUP()
#define PCP_SOCKET_ERROR (-1)
#define PCP_INVALID_SOCKET (-1)
#define CLOSE(sockfd) close(sockfd)

#endif
#endif

struct pcp_ctx_s;

extern pcp_socket_vt_t default_socket_vt;

void pcp_fill_in6_addr(struct in6_addr *dst_ip6, uint16_t *dst_port,
        struct sockaddr *src);

void pcp_fill_sockaddr(struct sockaddr *dst, struct in6_addr *sip,
        uint16_t sport, int ret_ipv6_mapped_ipv4, uint32_t scope_id);

PCP_SOCKET pcp_socket_create(struct pcp_ctx_s *ctx, int domain, int type,
        int protocol);

ssize_t pcp_socket_recvfrom(struct pcp_ctx_s *ctx, void *buf, size_t len,
        int flags, struct sockaddr *src_addr, socklen_t *addrlen);

ssize_t pcp_socket_sendto(struct pcp_ctx_s *ctx, const void *buf, size_t len,
        int flags, struct sockaddr *dest_addr, socklen_t addrlen);

int pcp_socket_close(struct pcp_ctx_s *ctx);

/*In Visual Studio inline keyword only available in C++ */
#if (defined(_MSC_VER) && !defined(inline))
#define inline __inline
#endif

#ifndef SA_LEN
#ifdef HAVE_SOCKADDR_SA_LEN
#define SA_LEN(addr)    ((addr)->sa_len)
#else /* HAVE_SOCKADDR_SA_LEN */

static inline size_t get_sa_len(struct sockaddr *addr)
{
    switch (addr->sa_family) {

        case AF_INET:
            return (sizeof(struct sockaddr_in));

        case AF_INET6:
            return (sizeof(struct sockaddr_in6));

        default:
            return (sizeof(struct sockaddr));
    }
}
#define SA_LEN(addr)    (get_sa_len(addr))
#endif /* HAVE_SOCKADDR_SA_LEN */
#endif /* SA_LEN */

#ifdef HAVE_SOCKADDR_SA_LEN
#define SET_SA_LEN(s, l) ((struct sockaddr*)s)->sa_len=l
#else
#define SET_SA_LEN(s, l)
#endif

#endif /* PCP_SOCKET_H*/
