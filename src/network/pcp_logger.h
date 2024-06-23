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

#ifndef PCP_LOGGER_H_
#define PCP_LOGGER_H_

#define ERR_BUF_LEN 256

#include "pcp.h"

#ifdef NDEBUG
#undef DEBUG
#endif

void pcp_logger_init(void);

#ifdef WIN32
void pcp_logger(pcp_loglvl_e log_level, const char* fmt, ...);
#else
void pcp_logger(pcp_loglvl_e log_level, const char* fmt, ...)
        __attribute__((format(printf, 2, 3)));
#endif

#ifdef DEBUG

#ifndef PCP_MAX_LOG_LEVEL
// Maximal log level for compile-time check
#define PCP_MAX_LOG_LEVEL PCP_LOGLVL_DEBUG
#endif

#define PCP_LOG(level, fmt, ...) { if (level<=PCP_MAX_LOG_LEVEL) \
pcp_logger(level, "FILE: %s:%d; Func: %s:\n     " fmt,\
__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__); }

#define PCP_LOG_END(level) { if (level<=PCP_MAX_LOG_LEVEL) \
pcp_logger(level, "FILE: %s:%d; Func: %s: END \n     " ,\
__FILE__, __LINE__, __FUNCTION__); }

#define PCP_LOG_BEGIN(level) { if (level<=PCP_MAX_LOG_LEVEL) \
pcp_logger(level, "FILE: %s:%d; Func: %s: BEGIN \n     ",\
__FILE__, __LINE__, __FUNCTION__); }

#else //DEBUG
#ifndef PCP_MAX_LOG_LEVEL
// Maximal log level for compile-time check
#define PCP_MAX_LOG_LEVEL PCP_LOGLVL_INFO
#endif

#define PCP_LOG(level, fmt, ...) { \
if (level<=PCP_MAX_LOG_LEVEL) pcp_logger(level, fmt, __VA_ARGS__); }

#define PCP_LOG_END(level)

#define PCP_LOG_BEGIN(level)

#endif // DEBUG
#if PCP_MAX_LOG_LEVEL>=PCP_LOGLVL_DEBUG
#define PCP_LOG_DEBUG(fmt, ...) PCP_LOG(PCP_LOGLVL_DEBUG, fmt, __VA_ARGS__)
#else
#define PCP_LOG_DEBUG(fmt, ...)
#endif

#if PCP_MAX_LOG_LEVEL>=PCP_LOGLVL_INFO
#define PCP_LOG_FLOW(f, msg) \
do { \
    if (pcp_log_level >= PCP_LOGLVL_INFO) { \
        char src_buf[INET6_ADDRSTRLEN]="Unknown"; \
        char dst_buf[INET6_ADDRSTRLEN]="Unknown"; \
        char pcp_buf[INET6_ADDRSTRLEN]="Unknown"; \
\
        inet_ntop(AF_INET6, &f->kd.src_ip, src_buf, sizeof(src_buf)); \
        inet_ntop(AF_INET6, &f->kd.map_peer.dst_ip, dst_buf, \
                sizeof(dst_buf)); \
        inet_ntop(AF_INET6, &f->kd.pcp_server_ip, pcp_buf, sizeof(pcp_buf)); \
        PCP_LOG(PCP_LOGLVL_INFO, \
                "%s(PCP server: %s; Int. addr: [%s]:%d; Dest. addr: [%s]:%d; Key bucket: %d)", \
                msg, pcp_buf, src_buf, ntohs(f->kd.map_peer.src_port), dst_buf, ntohs(f->kd.map_peer.dst_port), f->key_bucket); \
    } \
} while(0)
#else
#define PCP_LOG_FLOW(f, msg) do{} while(0)
#endif

void pcp_strerror(int errnum, char *buf, size_t buflen);

#endif /* PCP_LOGGER_H_ */
