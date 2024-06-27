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

#ifndef DEFAULT_CONFIG_H_
#define DEFAULT_CONFIG_H_

/* disable NATPMP support */
/* #undef PCP_DISABLE_NATPMP_SUPPORT */

/* enable experimental PCP options support */
/* #undef PCP_EXPERIMENTAL */

/* enable FLOW-PRIORITY option support */
/* #undef PCP_FLOW_PRIORITY */

/* Maximum number of ping attempts */
#ifndef PCP_MAX_PING_COUNT
#define PCP_MAX_PING_COUNT 5
#endif

/* Initial retransmission time */
#ifndef PCP_RETX_IRT
#define PCP_RETX_IRT 3000
#endif

/* Maximum retransmission count (0 indicates no maximum) */
#ifndef PCP_RETX_MRC
#define PCP_RETX_MRC 3
#endif

/* Maximum retransmission duration (0 indicates no maximum) */
#ifndef PCP_RETX_MRD
#define PCP_RETX_MRD 0
#endif

/* Maximum retransmission time */
#ifndef PCP_RETX_MRT
#define PCP_RETX_MRT 1024000
#endif

/* enable SADSCP option support */
/* #undef PCP_SADSCP */

/* Server discovery retry delay */
#ifndef PCP_SERVER_DISCOVERY_RETRY_DELAY
#define PCP_SERVER_DISCOVERY_RETRY_DELAY 3600
#endif

/* Default PCP server port */
#ifndef PCP_SERVER_PORT
#define PCP_SERVER_PORT 5351
#endif

#ifndef PCP_MAX_SUPPORTED_VERSION
#define PCP_MAX_SUPPORTED_VERSION 2
#endif

#ifndef PCP_MIN_SUPPORTED_VERSION
#ifdef PCP_DISABLE_NATPMP_SUPPORT
#define PCP_MIN_SUPPORTED_VERSION 1
#else
#define PCP_MIN_SUPPORTED_VERSION 0
#endif
#endif

#endif /* DEFAULT_CONFIG_H_ */
