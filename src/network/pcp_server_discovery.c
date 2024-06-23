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

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef WIN32
#include "windows/pcp_win_defines.h"
#else
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif //WIN32
#include "pcp.h"
#include "pcp_utils.h"
#include "pcp_server_discovery.h"
#include "pcp_event_handler.h"
#include "net/gateway.h"
#include "pcp_msg.h"
#include "pcp_logger.h"
#include "net/findsaddr.h"
#include "net/pcp_socket.h"

static pcp_errno psd_fill_pcp_server_src(pcp_server_t *s)
{
    struct in6_addr src_ip;
    const char *err=NULL;

    PCP_LOG_BEGIN(PCP_LOGLVL_DEBUG);

    if (!s) {
        PCP_LOG_END(PCP_LOGLVL_DEBUG);
        return PCP_ERR_BAD_ARGS;
    }

    memset(&s->pcp_server_saddr, 0, sizeof(s->pcp_server_saddr));
    memset(&src_ip, 0, sizeof(src_ip));

#ifndef PCP_USE_IPV6_SOCKET
    s->pcp_server_saddr.ss_family=AF_INET;
    if (s->af == AF_INET) {
        ((struct sockaddr_in *)&s->pcp_server_saddr)->sin_addr.s_addr=
                s->pcp_ip[3];
        ((struct sockaddr_in *)&s->pcp_server_saddr)->sin_port=s->pcp_port;
        SET_SA_LEN(&s->pcp_server_saddr, sizeof(struct sockaddr_in));
        inet_ntop(AF_INET,
                (void *)&((struct sockaddr_in *)&s->pcp_server_saddr)->sin_addr,
                s->pcp_server_paddr, sizeof(s->pcp_server_paddr));

        err=findsaddr((struct sockaddr_in *)&s->pcp_server_saddr, &src_ip);
        if (err) {
            PCP_LOG(PCP_LOGLVL_WARN,
                    "Error (%s) occurred while registering a new "
                    "PCP server %s", err, s->pcp_server_paddr);

            PCP_LOG_END(PCP_LOGLVL_DEBUG);
            return PCP_ERR_UNKNOWN;
        }
        s->src_ip[0]=0;

        s->src_ip[1]=0;
        s->src_ip[2]=htonl(0xFFFF);
        s->src_ip[3]=S6_ADDR32(&src_ip)[3];
    } else {
        PCP_LOG(PCP_LOGLVL_WARN, "%s",
                "IPv6 is disabled and IPv6 address of PCP server occurred");

        PCP_LOG_END(PCP_LOGLVL_DEBUG);
        return PCP_ERR_BAD_AFINET;
    }
#else //PCP_USE_IPV6_SOCKET
    s->pcp_server_saddr.ss_family=AF_INET6;
    if (s->af == AF_INET) {
        return PCP_ERR_BAD_AFINET; //should never happen
    }
    pcp_fill_sockaddr((struct sockaddr *)&s->pcp_server_saddr,
            (struct in6_addr *)&s->pcp_ip, s->pcp_port, 1, s->pcp_scope_id);

    inet_ntop(AF_INET6,
            (void *)&((struct sockaddr_in6*) &s->pcp_server_saddr)->sin6_addr,
            s->pcp_server_paddr, sizeof(s->pcp_server_paddr));

    err=findsaddr6((struct sockaddr_in6*)&s->pcp_server_saddr, &src_ip);
    if (err) {
        PCP_LOG(PCP_LOGLVL_WARN,
                "Error (%s) occurred while registering a new "
                "PCP server %s", err, s->pcp_server_paddr);

        PCP_LOG_END(PCP_LOGLVL_DEBUG);
        return PCP_ERR_UNKNOWN;
    }
    s->src_ip[0]=S6_ADDR32(&src_ip)[0];
    s->src_ip[1]=S6_ADDR32(&src_ip)[1];
    s->src_ip[2]=S6_ADDR32(&src_ip)[2];
    s->src_ip[3]=S6_ADDR32(&src_ip)[3];
#endif //PCP_USE_IPV6_SOCKET
    s->server_state=pss_ping;
    s->next_timeout.tv_sec=0;
    s->next_timeout.tv_usec=0;

    PCP_LOG_END(PCP_LOGLVL_DEBUG);
    return PCP_ERR_SUCCESS;
}

void psd_add_gws(pcp_ctx_t *ctx)
{
    struct sockaddr_in6 *gws=NULL, *gw;
    int rcount=getgateways(&gws);

    gw=gws;

    for (; rcount > 0; rcount--, gw++) {
        int pcps_indx;

        if ((IN6_IS_ADDR_V4MAPPED(&gw->sin6_addr)) && (S6_ADDR32(&gw->sin6_addr)[3] == INADDR_ANY))
            continue;

        if (IN6_IS_ADDR_UNSPECIFIED(&gw->sin6_addr))
            continue;

        if (get_pcp_server_by_ip(ctx, &gw->sin6_addr))
            continue;

        pcps_indx=pcp_new_server(ctx, &gw->sin6_addr, ntohs(PCP_SERVER_PORT), gw->sin6_scope_id);
        if (pcps_indx >= 0) {
            pcp_server_t *s=get_pcp_server(ctx, pcps_indx);
            if (!s)
                continue;

            if (psd_fill_pcp_server_src(s)) {
                PCP_LOG(PCP_LOGLVL_ERR,
                        "Failed to initialize gateway %s as a PCP server.",
                        s?s->pcp_server_paddr:"NULL pointer!!!");
            } else {
                PCP_LOG(PCP_LOGLVL_INFO, "Found gateway %s. "
                "Added as possible PCP server.",
                        s?s->pcp_server_paddr:"NULL pointer!!!");
            }
        }
    }
    free(gws);
}

pcp_errno psd_add_pcp_server(pcp_ctx_t *ctx, struct sockaddr *sa,
        uint8_t version)
{
    struct in6_addr pcp_ip=IN6ADDR_ANY_INIT;
    uint16_t pcp_port;
    uint32_t scope_id=0;
    pcp_server_t *pcps=NULL;

    PCP_LOG_BEGIN(PCP_LOGLVL_DEBUG);

    if (sa->sa_family == AF_INET) {
        S6_ADDR32(&pcp_ip)[0]=0;
        S6_ADDR32(&pcp_ip)[1]=0;
        S6_ADDR32(&pcp_ip)[2]=htonl(0xFFFF);
        S6_ADDR32(&pcp_ip)[3]=((struct sockaddr_in *)sa)->sin_addr.s_addr;
        pcp_port=((struct sockaddr_in *)sa)->sin_port;
    } else {
        IPV6_ADDR_COPY(&pcp_ip, &((struct sockaddr_in6*)sa)->sin6_addr);
        pcp_port=((struct sockaddr_in6 *)sa)->sin6_port;
        scope_id=((struct sockaddr_in6 *)sa)->sin6_scope_id;
    }

    if (!pcp_port) {
        pcp_port=ntohs(PCP_SERVER_PORT);
    }

    pcps=get_pcp_server_by_ip(ctx, (struct in6_addr *)&pcp_ip);
    if (!pcps) {
        int pcps_indx=pcp_new_server(ctx, &pcp_ip, pcp_port, scope_id);

        if (pcps_indx >= 0) {
            pcps=get_pcp_server(ctx, pcps_indx);
        }

        if (pcps == NULL) {
            PCP_LOG(PCP_LOGLVL_ERR, "%s", "Can't add PCP server.\n");
            PCP_LOG_END(PCP_LOGLVL_DEBUG);
            return PCP_ERR_UNKNOWN;
        }
    } else {
        pcps->pcp_port=pcp_port;
    }

    pcps->pcp_version=version;
    pcps->server_state=pss_allocated;

    if (psd_fill_pcp_server_src(pcps)) {
        pcps->server_state=pss_unitialized;
        PCP_LOG(PCP_LOGLVL_INFO, "Failed to add PCP server %s",
                pcps->pcp_server_paddr);

        PCP_LOG_END(PCP_LOGLVL_DEBUG);
        return PCP_ERR_UNKNOWN;
    }

    PCP_LOG(PCP_LOGLVL_INFO, "Added PCP server %s", pcps->pcp_server_paddr);

    PCP_LOG_END(PCP_LOGLVL_DEBUG);
    return (pcp_errno)pcps->index;
}
