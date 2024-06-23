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

#ifndef PCP_H
#define PCP_H

#ifdef WIN32
#include <winsock2.h>
#include <in6addr.h>
#include <ws2tcpip.h>
#include <time.h>
#if !defined ssize_t && defined _MSC_VER
typedef int ssize_t;
#endif
#else //WIN32
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PCP_SOCKET_IS_VOIDPTR
#define PCP_SOCKET void *
#else //PCP_SOCKET_IS_VOIDPTR
#ifdef WIN32
#define PCP_SOCKET SOCKET
#else //WIN32
#define PCP_SOCKET int
#endif //WIN32
#endif //PCP_SOCKET_IS_VOIDPTR

#ifdef PCP_EXPERIMENTAL
typedef struct pcp_userid_option *pcp_userid_option_p;
typedef struct pcp_deviceid_option *pcp_deviceid_option_p;
typedef struct pcp_location_option *pcp_location_option_p;
#endif //PCP_EXPERIMENTAL

typedef enum {
    PCP_ERR_SUCCESS=0,
    PCP_ERR_MAX_SIZE=-1,
    PCP_ERR_OPT_ALREADY_PRESENT=-2,
    PCP_ERR_BAD_AFINET=-3,
    PCP_ERR_SEND_FAILED=-4,
    PCP_ERR_RECV_FAILED=-5,
    PCP_ERR_UNSUP_VERSION=-6,
    PCP_ERR_NO_MEM=-7,
    PCP_ERR_BAD_ARGS=-8,
    PCP_ERR_UNKNOWN=-9,
    PCP_ERR_SHORT_LIFETIME_ERR=-10,
    PCP_ERR_TIMEOUT=-11,
    PCP_ERR_NOT_FOUND=-12,
    PCP_ERR_WOULDBLOCK=-13,
    PCP_ERR_ADDRINUSE=-14
} pcp_errno;

/* DEBUG levels */
typedef enum {
    PCP_LOGLVL_NONE=0,
    PCP_LOGLVL_ERR=1,
    PCP_LOGLVL_WARN=2,
    PCP_LOGLVL_INFO=3,
    PCP_LOGLVL_PERR=4,
    PCP_LOGLVL_DEBUG=5
} pcp_loglvl_e;

typedef void (*external_logger)(pcp_loglvl_e, const char *);

void pcp_set_loggerfn(external_logger ext_log);

// runtime level of logging
extern pcp_loglvl_e pcp_log_level;

typedef struct pcp_flow_s pcp_flow_t;
typedef struct pcp_ctx_s pcp_ctx_t;

typedef struct pcp_socket_vt_s {
    PCP_SOCKET (*sock_create)(int domain, int type, int protocol);
    ssize_t (*sock_recvfrom)(PCP_SOCKET sockfd, void *buf, size_t len,
            int flags, struct sockaddr *src_addr, socklen_t *addrlen);
    ssize_t (*sock_sendto)(PCP_SOCKET sockfd, const void *buf, size_t len,
            int flags, struct sockaddr *dest_addr, socklen_t addrlen);
    int (*sock_close)(PCP_SOCKET sockfd);
} pcp_socket_vt_t;

/*
 * Initialize library, optionally initiate auto-discovery of PCP servers
 *    autodiscovery  - enable/disable auto-discovery of PCP servers
 *    socket_vt      - optional - virt. table to override default socket functions.
 *                     Pointer has to be valid until pcp_terminate is called.
 *                     Pass NULL to use default socket functions
 *    return value   - pcp context used in other functions.
 */
#define ENABLE_AUTODISCOVERY  1
#define DISABLE_AUTODISCOVERY 0
pcp_ctx_t *pcp_init(uint8_t autodiscovery, pcp_socket_vt_t *socket_vt);

//returns internal pcp server ID, -1 => error occurred
int pcp_add_server(pcp_ctx_t *ctx, struct sockaddr *pcp_server,
        uint8_t pcp_version);

/*
 * Close socket fds and clean up all settings, frees all library buffers
 *      close_flows - signal end of flows to PCP servers
 */
void pcp_terminate(pcp_ctx_t *ctx, int close_flows);

////////////////////////////////////////////////////////////////////////////////
//                          Flow API

/*
 * Creates new PCP message from parameters parsed to this function:
 *  src_addr    source IP/port
 *  dst_addr    destination IP/port - optional
 *  ext_addr    sugg. ext. IP/port  - optional
 *  protocol    protocol associated with flow
 *  lifetime    time in seconds how long should mapping last
 *  userdata    pointer to user data associated with a new flow
 *
 *  return value
 *  pcp_flow_t *used in other functions to reference this flow.
 */
pcp_flow_t *pcp_new_flow(pcp_ctx_t *ctx, struct sockaddr *src_addr,
        struct sockaddr *dst_addr, struct sockaddr *ext_addr, uint8_t protocol,
        uint32_t lifetime, void *userdata);

void pcp_flow_set_lifetime(pcp_flow_t *f, uint32_t lifetime);

/*
 * Store pointer to application's user data associated with a flow. Pointer can
 * be read by function pcp_flow_get_user_data. Arguments:
 *  f           flow - to which store application's user data.
 *  userdata    pointer to user data to be stored along a flow
 */
void pcp_flow_set_user_data(pcp_flow_t *f, void *userdata);

/*
 * Get a pointer to application's user data associated with the flow, previously
 * stored by pcp_flow_set_user_data, or pcp_new_flow. Arguments:
 *  f           flow - to which store application's user data.
 *
 *  return
 *  void *      pointer to user data to be stored along a flow
 */
void *pcp_flow_get_user_data(pcp_flow_t *f);

/*
 * Set 3rd party option to the existing message flow info.
 */
void pcp_flow_set_3rd_party_opt(pcp_flow_t *f, struct sockaddr *thirdp_addr);

#ifdef PCP_FLOW_PRIORITY
/*
 * Set PCP's flow priority option to the flow, causing a router to start
 * altering IP packets' DSCP field to dscp_up in up direction and to dscp_down
 * for down direction.
 */
void pcp_flow_set_flowp(pcp_flow_t *f, uint8_t dscp_up, uint8_t dscp_down);
#endif

#ifdef PCP_EXPERIMENTAL
/*
 * Append metadata option to the existing flow f
 * if exists md with given id then replace with new value
 * if value is NULL then remove metadata with this id
 */
void pcp_flow_add_md(pcp_flow_t *f, uint32_t md_id, void *value, size_t val_len);

int pcp_flow_set_userid(pcp_flow_t *f, pcp_userid_option_p userid);
int pcp_flow_set_deviceid(pcp_flow_t *f, pcp_deviceid_option_p dev);
int pcp_flow_set_location(pcp_flow_t *f, pcp_location_option_p loc);
#endif

/*
 * Append filter option.
 */
void pcp_flow_set_filter_opt(pcp_flow_t *f, struct sockaddr *filter_ip,
        uint8_t filter_prefix);

/*
 * Append prefer failure option.
 */
void pcp_flow_set_prefer_failure_opt(pcp_flow_t *f);

#ifdef PCP_SADSCP
/*
 * create new PCP message with SADSCP opcode. It's used to learn
 * correct DSCP values to get desired flow treatment by router.
 */
pcp_flow_t *pcp_learn_dscp(pcp_ctx_t *ctx, uint8_t delay_tol, uint8_t loss_tol,
        uint8_t jitter_tol, char *app_name);
#endif

/*
 * Remove flow from PCP server. Sends a PCP message with lifetime set to 0
 * to PCP server.
 */
void pcp_close_flow(pcp_flow_t *f);

/*
 * Frees memory pointed to by f; Invalidates f.
 */
void pcp_delete_flow(pcp_flow_t *f);

typedef enum {
    pcp_state_processing,
    pcp_state_succeeded,
    pcp_state_partial_result,
    pcp_state_short_lifetime_error,
    pcp_state_failed
} pcp_fstate_e;

typedef struct pcp_flow_info {
    pcp_fstate_e     result;
    struct in6_addr  pcp_server_ip;
    struct in6_addr  ext_ip;
    uint16_t         ext_port;     //network byte order
    time_t           recv_lifetime_end;
    time_t           lifetime_renew_s;
    uint8_t          pcp_result_code;
    struct in6_addr  int_ip;
    uint16_t         int_port;     //network byte order
    struct in6_addr  dst_ip;
    uint16_t         dst_port;     //network byte order
    uint8_t          protocol;
    uint8_t          learned_dscp; //relevant only for flow created by pcp_learn_dscp
} pcp_flow_info_t;

// Allocates info_buf by malloc, has to be freed by client when no longer needed.
pcp_flow_info_t *pcp_flow_get_info(pcp_flow_t *f, size_t *info_count);

//callback function type - called when flow state has changed
typedef void (*pcp_flow_change_notify)(pcp_flow_t *f, struct sockaddr *src_addr,
        struct sockaddr *ext_addr, pcp_fstate_e, void *cb_arg);

//set flow state change notify callback function
void pcp_set_flow_change_cb(pcp_ctx_t *ctx, pcp_flow_change_notify cb_fun,
        void *cb_arg);

/* evaluate flow state
 * params:
 *   flow (in)    - handle of the flow
 *   fstate (out) - state of the flow
 *   return value - count of interfaces in exit_state (nonzero value means
 *                  there is some result from PCP server)
 */
int pcp_eval_flow_state(pcp_flow_t *flow, pcp_fstate_e *fstate);

////////////////////////////////////////////////////////////////////////////////
//                      Event handling functions

/*
 * pcp_pulse - handle socket and timeout events. It's intended to be
 * used in select loop.
 * params:
 *   pcp_ctx_t *ctx       - pcp context obtained by pcp_init
 *   next_timeout(in/out) - nearest time-out. if it is filled by nonzero
 *                          value it will return smaller one of provided and
 *                          inner calculated. to be used in select function
 */
int pcp_pulse(pcp_ctx_t *ctx, struct timeval *next_timeout);

/*
 * Get socket used to communicate with PCP server.
 */
PCP_SOCKET pcp_get_socket(pcp_ctx_t *ctx);

//example of pcp_pulse and pcp_get_socket use in select loop:
/*
 pcp_ctx_t *ctx=pcp_init(1, NULL);
 int sock=pcp_get_socket(ctx);
 pcp_flow_t f=pcp_new_flow(ctx,...);
 fd_set rfds;
 do {
   struct timeval tv={0, 0};
   FD_ZERO(&rfds);
   FD_SET(sock, &rfds);
   pcp_pulse(ctx, &tv);
   select(sock+1, &rfds, NULL, NULL, &tv);
 } while (1);
 */

////////////////////////////////////////////////////////////////////////////////
// Blocking wait for flow reaching one of exit states or time-out(ms)
// expiration.
/*   pcp_wait
 * params:
 *   flow    (in)             - pcp flow handle
 *   timeout (in)             - maximal time in ms to wait for result
 *   exit_on_partial_res (in) - do not wait for result of all interfaces or
 *                              possible PCP servers. Instead return immediately
 *                              after first received result.
 */
pcp_fstate_e pcp_wait(pcp_flow_t *flow, int timeout, int exit_on_partial_res);

// example of pcp_wait use:
/*
    pcp_flow_t f=pcp_new_flow(ctx, (struct sockaddr *)&src,
                    (struct sockaddr *)&dst, NULL, IPPROTO_TCP, 60, NULL);
    pcp_flow_set_flowp(f, 12, 16);
    pcp_wait(f, 500, 0);  // send PCP msg and wait for response for 500 ms
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PCP_H */
