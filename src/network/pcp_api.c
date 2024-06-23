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

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS 1
#endif

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#ifdef WIN32
#include <winsock2.h>
#include "windows/pcp_win_defines.h"
#include "windows/pcp_gettimeofday.h"
#else
#include <sys/select.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif
#include "pcp.h"
#include "net/pcp_socket.h"
#include "pcp_client_db.h"
#include "pcp_logger.h"
#include "pcp_event_handler.h"
#include "pcp_utils.h"
#include "pcp_server_discovery.h"
#include "net/findsaddr.h"

PCP_SOCKET pcp_get_socket(pcp_ctx_t *ctx)
{

    return ctx ? ctx->socket : PCP_INVALID_SOCKET;
}

int pcp_add_server(pcp_ctx_t *ctx, struct sockaddr *pcp_server,
        uint8_t pcp_version)
{
    int res;

    PCP_LOG_BEGIN(PCP_LOGLVL_DEBUG);

    if (!ctx) {
        return PCP_ERR_BAD_ARGS;
    }
    if (pcp_version > PCP_MAX_SUPPORTED_VERSION) {
        PCP_LOG_END(PCP_LOGLVL_INFO);
        return PCP_ERR_UNSUP_VERSION;
    }

    res=psd_add_pcp_server(ctx, pcp_server, pcp_version);

    PCP_LOG_END(PCP_LOGLVL_DEBUG);
    return res;
}

pcp_ctx_t *pcp_init(uint8_t autodiscovery, pcp_socket_vt_t *socket_vt)
{
    pcp_ctx_t *ctx=(pcp_ctx_t *)calloc(1, sizeof(pcp_ctx_t));

    pcp_logger_init();

    PCP_LOG_BEGIN(PCP_LOGLVL_DEBUG);

    if (!ctx) {
        PCP_LOG_END(PCP_LOGLVL_DEBUG);
        return NULL;
    }

    if (socket_vt) {
        ctx->virt_socket_tb=socket_vt;
    } else {
        ctx->virt_socket_tb=&default_socket_vt;
    }

    ctx->socket=pcp_socket_create(ctx,
#ifdef PCP_USE_IPV6_SOCKET
            AF_INET6,
#else
            AF_INET,
#endif
            SOCK_DGRAM, 0);

    if (ctx->socket == PCP_INVALID_SOCKET) {
        PCP_LOG(PCP_LOGLVL_WARN, "%s",
                "Error occurred while creating a PCP socket.");

        PCP_LOG_END(PCP_LOGLVL_DEBUG);
        return NULL;
    }
    PCP_LOG(PCP_LOGLVL_DEBUG, "%s", "Created a new PCP socket.");

    if (autodiscovery)
        psd_add_gws(ctx);

    PCP_LOG_END(PCP_LOGLVL_DEBUG);
    return ctx;
}

int pcp_eval_flow_state(pcp_flow_t *flow, pcp_fstate_e *fstate)
{
    pcp_flow_t *fiter;
    int nexit_states=0;
    int fpresent_no_exit_state=0;
    int fsuccess=0;
    int ffailed=0;

    PCP_LOG_BEGIN(PCP_LOGLVL_DEBUG);

    for (fiter=flow; fiter != NULL; fiter=fiter->next_child) {
        switch (fiter->state) {
            case pfs_wait_for_lifetime_renew:
                fsuccess=1;
                ++nexit_states;
                break;
            case pfs_failed:
                ffailed=1;
                ++nexit_states;
                break;
            case pfs_wait_after_short_life_error:
                ++nexit_states;
                break;
            default:
                fpresent_no_exit_state=1;
                break;
        }
    }

    if (fstate) {
        if (fpresent_no_exit_state) {
            if (fsuccess) {
                *fstate=pcp_state_partial_result;
            } else {
                *fstate=pcp_state_processing;
            }
        } else {
            if (fsuccess) {
                *fstate=pcp_state_succeeded;
            } else if (ffailed) {
                *fstate=pcp_state_failed;
            } else {
                *fstate=pcp_state_short_lifetime_error;
            }
        }
    }

    PCP_LOG_END(PCP_LOGLVL_DEBUG);
    return nexit_states;
}

pcp_fstate_e pcp_wait(pcp_flow_t *flow, int timeout, int exit_on_partial_res)
{
#ifdef PCP_SOCKET_IS_VOIDPTR
    return pcp_state_failed;
#else
    fd_set read_fds;
    int fdmax;
    PCP_SOCKET fd;
    struct timeval tout_end;
    struct timeval tout_select;
    pcp_fstate_e fstate;
    int nflow_exit_states=pcp_eval_flow_state(flow, &fstate);

    PCP_LOG_BEGIN(PCP_LOGLVL_DEBUG);

    if (!flow) {
        PCP_LOG(PCP_LOGLVL_PERR, "Flow argument of %s function set to NULL!",
                __FUNCTION__);
        return pcp_state_failed;
    }

    switch (fstate) {
        case pcp_state_partial_result:
        case pcp_state_processing:
            break;
        default:
            nflow_exit_states=0;
            break;
    }

    gettimeofday(&tout_end, NULL);
    tout_end.tv_usec+=(timeout * 1000) % 1000000;
    tout_end.tv_sec+=tout_end.tv_usec / 1000000;
    tout_end.tv_usec=tout_end.tv_usec % 1000000;
    tout_end.tv_sec+=timeout / 1000;

    PCP_LOG(PCP_LOGLVL_INFO,
            "Initialized wait for result of flow: %d, wait timeout %d ms",
            flow->key_bucket, timeout);

    FD_ZERO(&read_fds);

    fd=pcp_get_socket(flow->ctx);
    fdmax=fd + 1;

    // main loop
    for (;;) {
        int ret_count;
        pcp_fstate_e ret_state;
        struct timeval ctv;

        OSDEP(ret_count);
        // check expiration of wait timeout
        gettimeofday(&ctv, NULL);
        if ((timeval_subtract(&tout_select, &tout_end, &ctv))
                || ((tout_select.tv_sec == 0) && (tout_select.tv_usec == 0))
                || (tout_select.tv_sec < 0)) {
            return pcp_state_processing;
        }

        //process all events and get timeout value for next select
        pcp_pulse(flow->ctx, &tout_select);

        // check flow for reaching one of exit from wait states
        // (also handles case when flow is MAP for 0.0.0.0)
        if (pcp_eval_flow_state(flow, &ret_state) > nflow_exit_states) {
            if ((exit_on_partial_res)
                    || (ret_state != pcp_state_partial_result)) {
                PCP_LOG_END(PCP_LOGLVL_DEBUG);
                return ret_state;
            }
        }

        FD_ZERO(&read_fds);
        FD_SET(fd, &read_fds);

        PCP_LOG(PCP_LOGLVL_DEBUG,
                "Executing select with fdmax=%d, timeout = %ld s; %ld us",
                fdmax, tout_select.tv_sec, (long int)tout_select.tv_usec);

        ret_count=select(fdmax, &read_fds, NULL, NULL, &tout_select);

        // check of select result // only for debug purposes
#ifdef DEBUG
        if (ret_count == -1) {
            char error[ERR_BUF_LEN];
            pcp_strerror(errno, error, sizeof(error));
            PCP_LOG(PCP_LOGLVL_PERR,
                    "select failed: %s", error);
        } else if (ret_count == 0) {
            PCP_LOG(PCP_LOGLVL_DEBUG, "%s",
                    "select timed out");
        } else {
            PCP_LOG(PCP_LOGLVL_DEBUG,
                    "select returned %d i/o events.", ret_count);
        }
#endif
    }PCP_LOG_END(PCP_LOGLVL_DEBUG);
    //return pcp_state_succeeded;
#endif //PCP_SOCKET_IS_VOIDPTR
}

static inline void init_flow(pcp_flow_t *f, pcp_server_t *s, int lifetime,
        struct sockaddr *ext_addr)
{
    PCP_LOG_BEGIN(PCP_LOGLVL_DEBUG);
    if (f && s) {
        struct timeval curtime;
        f->ctx=s->ctx;

        switch (f->kd.operation) {
          case PCP_OPCODE_MAP:
          case PCP_OPCODE_PEER:
              pcp_fill_in6_addr(&f->map_peer.ext_ip, &f->map_peer.ext_port,
                  ext_addr);
              break;
          default:
            assert(!ext_addr);
            break;
        }

        gettimeofday(&curtime, NULL);
        f->lifetime=lifetime;
        f->timeout=curtime;

        if (s->server_state == pss_wait_io) {
            f->state=pfs_send;
        } else {
            f->state=pfs_wait_for_server_init;
        }

        s->next_timeout=curtime;
        f->user_data=NULL;

        pcp_db_add_flow(f);
        PCP_LOG_FLOW(f, "Added new flow");
    }
    PCP_LOG_END(PCP_LOGLVL_DEBUG);
}

struct caasi_data {
    struct flow_key_data *kd;
    pcp_flow_t *fprev;
    pcp_flow_t *ffirst;
    uint32_t lifetime;
    struct sockaddr *ext_addr;
    struct in6_addr *src_ip;
    uint8_t toler_fields;
    char *app_name;
    void *userdata;
};

static int chain_and_assign_src_ip(pcp_server_t *s, void *data)
{
    struct caasi_data *d=(struct caasi_data *)data;

    PCP_LOG_BEGIN(PCP_LOGLVL_DEBUG);

    if (s->server_state == pss_not_working) {
        PCP_LOG_END(PCP_LOGLVL_DEBUG);
        return 0;
    }

    if ((IN6_IS_ADDR_UNSPECIFIED(d->src_ip))
            || (IN6_ARE_ADDR_EQUAL(d->src_ip, (struct in6_addr *) s->src_ip))) {
        pcp_flow_t *f=NULL;

        memcpy(&d->kd->src_ip, s->src_ip, sizeof(d->kd->src_ip));
        memcpy(&d->kd->pcp_server_ip, s->pcp_ip, sizeof(d->kd->pcp_server_ip));
        memcpy(&d->kd->nonce, &s->nonce, sizeof(d->kd->nonce));

        f=pcp_create_flow(s, d->kd);
        if (!f) {
            PCP_LOG_END(PCP_LOGLVL_DEBUG);
            return 1;
        }
#ifdef PCP_SADSCP
        if (d->kd->operation == PCP_OPCODE_SADSCP) {
            f->sadscp.toler_fields = d->toler_fields;
            if (d->app_name) {
                f->sadscp.app_name_length = strlen(d->app_name);
                f->sadscp_app_name = strdup(d->app_name);
            } else {
                f->sadscp.app_name_length = 0;
                f->sadscp_app_name = NULL;
            }
        }
#endif
        init_flow(f, s, d->lifetime, d->ext_addr);
        f->user_data=d->userdata;
        if (d->fprev) {
            d->fprev->next_child=f;
        } else {
            d->ffirst=f;
        }
        d->fprev=f;
    }

    PCP_LOG_END(PCP_LOGLVL_DEBUG);
    return 0;
}

pcp_flow_t *pcp_new_flow(pcp_ctx_t *ctx, struct sockaddr *src_addr,
        struct sockaddr *dst_addr, struct sockaddr *ext_addr, uint8_t protocol,
        uint32_t lifetime, void *userdata)
{
    struct flow_key_data kd;
    struct caasi_data data;
    struct in6_addr src_ip;
    struct sockaddr_storage tmp_ext_addr;

    PCP_LOG_BEGIN(PCP_LOGLVL_DEBUG);

    memset(&kd, 0, sizeof(kd));

    if ((!src_addr) || (!ctx)) {
        return NULL;
    }
    pcp_fill_in6_addr(&src_ip, &kd.map_peer.src_port, src_addr);

    kd.map_peer.protocol=protocol;

    if (dst_addr) {
        switch (dst_addr->sa_family) {
            case AF_INET:
                if (((struct sockaddr_in*)(dst_addr))->sin_addr.s_addr
                        == INADDR_ANY) {
                    dst_addr=NULL;
                }
                break;
            case AF_INET6:
                if (IN6_IS_ADDR_UNSPECIFIED(
                        &((struct sockaddr_in6 *)(dst_addr))->sin6_addr)) {
                    dst_addr=NULL;
                }
                break;
            default:
                dst_addr=NULL;
                break;
        }
    }

    if (dst_addr) {
        pcp_fill_in6_addr(&kd.map_peer.dst_ip, &kd.map_peer.dst_port, dst_addr);
        kd.operation=PCP_OPCODE_PEER;
        if (src_addr->sa_family == AF_INET) {
            if (S6_ADDR32(&src_ip)[3] == INADDR_ANY) {
                findsaddr((struct sockaddr_in*)dst_addr, &src_ip);
            }
        } else if (IN6_IS_ADDR_UNSPECIFIED(&src_ip)) {
            findsaddr6((struct sockaddr_in6*)dst_addr, &src_ip);
        } else if (dst_addr->sa_family != src_addr->sa_family) {
            PCP_LOG(PCP_LOGLVL_PERR, "%s",
                    "Socket family mismatch.");

            PCP_LOG_END(PCP_LOGLVL_DEBUG);
            return NULL;
        }
    } else {
        kd.operation=PCP_OPCODE_MAP;
    }

    if (!ext_addr) {
      struct sockaddr_in *te4=(struct sockaddr_in *)&tmp_ext_addr;
      struct sockaddr_in6 *te6=(struct sockaddr_in6 *)&tmp_ext_addr;
      tmp_ext_addr.ss_family=src_addr->sa_family;
      switch (tmp_ext_addr.ss_family) {
        case AF_INET:
          memset(&te4->sin_addr, 0, sizeof(te4->sin_addr));
          te4->sin_port=0;
          break;
        case AF_INET6:
          memset(&te6->sin6_addr, 0, sizeof(te6->sin6_addr));
          te6->sin6_port=0;
          break;
        default:
          PCP_LOG(PCP_LOGLVL_PERR, "%s",
                  "Unsupported address family.");
          return NULL;
      }
      ext_addr=(struct sockaddr *)&tmp_ext_addr;
    }

    data.fprev=NULL;
    data.lifetime=lifetime;
    data.ext_addr=ext_addr;
    data.src_ip=&src_ip;
    data.kd=&kd;
    data.ffirst=NULL;
    data.userdata=userdata;

    if (pcp_db_foreach_server(ctx, chain_and_assign_src_ip, &data)
            != PCP_ERR_MAX_SIZE) {    // didn't iterate through each server => error happened
        pcp_delete_flow(data.ffirst);
        PCP_LOG_END(PCP_LOGLVL_DEBUG);
        return NULL;
    }

    PCP_LOG_END(PCP_LOGLVL_DEBUG);
    return data.ffirst;
}

void pcp_flow_set_lifetime(pcp_flow_t *f, uint32_t lifetime)
{
    pcp_flow_t *fiter;

    for (fiter=f; fiter != NULL; fiter=fiter->next_child) {
        fiter->lifetime=lifetime;

        pcp_flow_updated(fiter);
    }
}

void pcp_flow_set_3rd_party_opt(pcp_flow_t *f, struct sockaddr *thirdp_addr)
{
    pcp_flow_t *fiter;

    for (fiter=f; fiter != NULL; fiter=fiter->next_child) {
        fiter->third_party_option_present=1;
        pcp_fill_in6_addr(&fiter->third_party_ip, NULL, thirdp_addr);
        pcp_flow_updated(fiter);
    }
}

void pcp_flow_set_filter_opt(pcp_flow_t *f, struct sockaddr *filter_ip,
        uint8_t filter_prefix)
{
    pcp_flow_t *fiter;

    for (fiter=f; fiter != NULL; fiter=fiter->next_child) {
        if (!fiter->filter_option_present) {
            fiter->filter_option_present=1;
        }
        pcp_fill_in6_addr(&fiter->filter_ip, &fiter->filter_port, filter_ip);
        fiter->filter_prefix=filter_prefix;
        pcp_flow_updated(fiter);
    }
}

void pcp_flow_set_prefer_failure_opt(pcp_flow_t *f)
{
    pcp_flow_t *fiter;

    for (fiter=f; fiter != NULL; fiter=fiter->next_child) {
        if (!fiter->pfailure_option_present) {
            fiter->pfailure_option_present=1;
            pcp_flow_updated(fiter);
        }
    }
}
#ifdef PCP_EXPERIMENTAL
int pcp_flow_set_userid(pcp_flow_t *f, pcp_userid_option_p user)
{
    pcp_flow_t *fiter;

    for (fiter=f; fiter; fiter=fiter->next_child) {
        memcpy(&(fiter->f_userid.userid[0]), &(user->userid[0]), MAX_USER_ID);
        pcp_flow_updated(fiter);
    }
    return 0;
}

int pcp_flow_set_location(pcp_flow_t *f, pcp_location_option_p loc)
{
    pcp_flow_t *fiter;

    for (fiter=f; fiter; fiter=fiter->next_child) {
        memcpy(&(fiter->f_location.location[0]), &(loc->location[0]), MAX_GEO_STR);
        pcp_flow_updated(fiter);
    }

    return 0;
}

int pcp_flow_set_deviceid(pcp_flow_t *f, pcp_deviceid_option_p dev)
{
    pcp_flow_t *fiter;

    for (fiter=f; fiter; fiter=fiter->next_child) {
        memcpy(&(fiter->f_deviceid.deviceid[0]), &(dev->deviceid[0]), MAX_DEVICE_ID);
        pcp_flow_updated(fiter);
    }
    return 0;
}

void
pcp_flow_add_md (pcp_flow_t *f, uint32_t md_id, void *value, size_t val_len)
{
    pcp_flow_t *fiter;

    for (fiter=f; fiter!=NULL; fiter=fiter->next_child) {
        pcp_db_add_md(fiter, md_id, value, val_len);
        pcp_flow_updated(fiter);
    }
}
#endif

#ifdef PCP_FLOW_PRIORITY
void pcp_flow_set_flowp(pcp_flow_t *f, uint8_t dscp_up, uint8_t dscp_down)
{
    pcp_flow_t *fiter;

    for (fiter=f; fiter; fiter=fiter->next_child) {
        uint8_t fpresent = (dscp_up!=0)||(dscp_down!=0);
        if (fiter->flowp_option_present != fpresent) {
            fiter->flowp_option_present=fpresent;
        }
        if (fpresent) {
            fiter->flowp_dscp_up=dscp_up;
            fiter->flowp_dscp_down=dscp_down;
        }
        pcp_flow_updated(fiter);
    }
}
#endif

static inline void pcp_close_flow_intern(pcp_flow_t *f)
{
    switch (f->state) {
        case pfs_wait_for_server_init:
        case pfs_idle:
        case pfs_failed:
            f->state=pfs_failed;
            break;
        default:
            f->lifetime=0;
            pcp_flow_updated(f);
            break;
    }
    if ((f->state != pfs_wait_for_server_init) && (f->state != pfs_idle)
            && (f->state != pfs_failed)) {
        PCP_LOG_FLOW(f, "Flow closed");
        f->lifetime=0;
        pcp_flow_updated(f);
    } else {
        f->state=pfs_failed;
    }
}

void pcp_close_flow(pcp_flow_t *f)
{
    pcp_flow_t *fiter;

    for (fiter=f; fiter; fiter=fiter->next_child) {
        pcp_close_flow_intern(fiter);
    }

    if (f) {
        pcp_pulse(f->ctx, NULL);
    }
}

void pcp_delete_flow(pcp_flow_t *f)
{
    pcp_flow_t *fiter=f;
    pcp_flow_t *fnext=NULL;

    while (fiter) {
        fnext=fiter->next_child;
        pcp_delete_flow_intern(fiter);
        fiter=fnext;
    }
}

static int delete_flow_iter(pcp_flow_t *f, void *data)
{
    if (data) {
        pcp_close_flow_intern(f);
        pcp_pulse(f->ctx, NULL);
    }
    pcp_delete_flow_intern(f);

    return 0;
}

void pcp_terminate(pcp_ctx_t *ctx, int close_flows)
{
    pcp_db_foreach_flow(ctx, delete_flow_iter, close_flows ? (void *)1 : NULL);
    pcp_db_free_pcp_servers(ctx);
    pcp_socket_close(ctx);
}

pcp_flow_info_t *pcp_flow_get_info(pcp_flow_t *f, size_t *info_count)
{
    pcp_flow_t *fiter;
    pcp_flow_info_t *info_buf;
    pcp_flow_info_t *info_iter;
    uint32_t cnt=0;

    if (!info_count) {
        return NULL;
    }

    for (fiter=f; fiter; fiter=fiter->next_child) {
      ++cnt;
    }

    info_buf=(pcp_flow_info_t *)calloc(cnt, sizeof(pcp_flow_info_t));
    if (!info_buf) {
        PCP_LOG(PCP_LOGLVL_DEBUG, "%s", "Error allocating memory");
        return NULL;
    }

    for (fiter=f, info_iter=info_buf; fiter != NULL;
            fiter=fiter->next_child, ++info_iter) {

        switch (fiter->state) {
            case pfs_wait_after_short_life_error:
                info_iter->result=pcp_state_short_lifetime_error;
                break;
            case pfs_wait_for_lifetime_renew:
                info_iter->result=pcp_state_succeeded;
                break;
            case pfs_failed:
                info_iter->result=pcp_state_failed;
                break;
            default:
                info_iter->result=pcp_state_processing;
                break;
        }

        info_iter->recv_lifetime_end=fiter->recv_lifetime;
        info_iter->lifetime_renew_s=fiter->lifetime;
        info_iter->pcp_result_code=fiter->recv_result;
        memcpy(&info_iter->int_ip, &fiter->kd.src_ip, sizeof(struct in6_addr));
        memcpy(&info_iter->pcp_server_ip, &fiter->kd.pcp_server_ip,
                sizeof(info_iter->pcp_server_ip));
        if ((fiter->kd.operation == PCP_OPCODE_MAP)
                || (fiter->kd.operation == PCP_OPCODE_PEER)) {
            memcpy(&info_iter->dst_ip, &fiter->kd.map_peer.dst_ip,
                    sizeof(info_iter->dst_ip));
            memcpy(&info_iter->ext_ip, &fiter->map_peer.ext_ip,
                    sizeof(info_iter->ext_ip));
            info_iter->int_port=fiter->kd.map_peer.src_port;
            info_iter->dst_port=fiter->kd.map_peer.dst_port;
            info_iter->ext_port=fiter->map_peer.ext_port;
            info_iter->protocol=fiter->kd.map_peer.protocol;
#ifdef PCP_SADSCP
        } else if (fiter->kd.operation == PCP_OPCODE_SADSCP) {
            info_iter->learned_dscp=fiter->sadscp.learned_dscp;
#endif
        }
    }
    *info_count=cnt;

    return info_buf;
}

void pcp_flow_set_user_data(pcp_flow_t *f, void *userdata)
{
    pcp_flow_t *fiter=f;

    while (fiter) {
        fiter->user_data=userdata;
        fiter=fiter->next_child;
    }
}

void *pcp_flow_get_user_data(pcp_flow_t *f)
{
    return (f ? f->user_data : NULL);
}

#ifdef PCP_SADSCP
pcp_flow_t *pcp_learn_dscp(pcp_ctx_t *ctx, uint8_t delay_tol, uint8_t loss_tol,
        uint8_t jitter_tol, char *app_name)
{
    struct flow_key_data kd;
    struct caasi_data data;
    struct in6_addr src_ip=IN6ADDR_ANY_INIT;

    memset(&data, 0 ,sizeof(data));
    memset(&kd, 0 ,sizeof(kd));

    kd.operation=PCP_OPCODE_SADSCP;

    data.fprev=NULL;
    data.src_ip=&src_ip;
    data.kd=&kd;
    data.ffirst=NULL;
    data.lifetime=0;
    data.ext_addr=NULL;
    data.toler_fields=(delay_tol&3)<<6 | ((loss_tol&3)<<4)
        | ((jitter_tol&3)<<2);
    data.app_name=app_name;

    pcp_db_foreach_server(ctx, chain_and_assign_src_ip, &data);

    return data.ffirst;
}
#endif
