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

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "pcp.h"
#include "pcp_utils.h"
#include "pcp_client_db.h"
#include "pcp_logger.h"

#define EMPTY 0xFFFFFFFF
#define PCP_INIT_SERVER_COUNT 5

static uint32_t compute_flow_key(struct flow_key_data *kd)
{
    uint32_t h=0;
    uint8_t *k=(uint8_t*)(kd + 1);

    while ((void*)(k--) != (void*)kd) {
        uint32_t ho=h & 0xff000000;
        h=h << 8;
        h=h ^ (ho >> 24);
        h=h ^ *k;
    }

    h=(h * 0x9E3779B9) >> (32 - FLOW_HASH_BITS);

    return h;
}

pcp_flow_t *pcp_create_flow(pcp_server_t *s, struct flow_key_data *fkd)
{
    pcp_flow_t *flow;

    PCP_LOG_BEGIN(PCP_LOGLVL_DEBUG);

    assert(fkd && s);

    flow=(pcp_flow_t*)calloc(1, sizeof(struct pcp_flow_s));
    if (flow == NULL) {
        PCP_LOG(PCP_LOGLVL_ERR, "%s",
                "Malloc can't allocate enough memory for the pcp_flow.");
        PCP_LOG(PCP_LOGLVL_ERR, "%s", "Returning NULL.");
        PCP_LOG_END(PCP_LOGLVL_DEBUG);
        return NULL;
    }

    flow->pcp_msg_len=0;
    flow->pcp_server_indx=(s ? s->index : PCP_INV_SERVER);
    flow->kd=*fkd;
    flow->key_bucket=EMPTY;
    flow->ctx=s->ctx;

    PCP_LOG_END(PCP_LOGLVL_DEBUG);
    return flow;
}

void pcp_flow_clear_msg_buf(pcp_flow_t *f)
{
    if (f) {
        if (f->pcp_msg_buffer) {
            free(f->pcp_msg_buffer);
            f->pcp_msg_buffer=NULL;
        }
        f->pcp_msg_len=0;
    }
}

pcp_errno pcp_delete_flow_intern(pcp_flow_t *f)
{
    pcp_server_t *s;

    assert(f);

    pcp_db_rem_flow(f);

    if (f->pcp_msg_buffer) {
        free(f->pcp_msg_buffer);
    }

#ifdef PCP_EXPERIMENTAL
    if (f->md_vals) {
        free(f->md_vals);
    }
#endif

#ifdef PCP_SADSCP
    if (f->sadscp_app_name) {
        free(f->sadscp_app_name);
    }
#endif

    if ((f->pcp_server_indx != PCP_INV_SERVER)
            && ((s=get_pcp_server(f->ctx, f->pcp_server_indx)) != NULL)
            && (s->ping_flow_msg == f)) {
        s->ping_flow_msg=NULL;
    }

    free(f);
    return PCP_ERR_SUCCESS;
}

pcp_errno pcp_db_add_flow(pcp_flow_t *f)
{
    uint32_t indx;
    pcp_flow_t **fdb;
    pcp_ctx_t *ctx;

    if (!f) {
        return PCP_ERR_BAD_ARGS;
    }

    ctx=f->ctx;

    f->key_bucket=indx=compute_flow_key(&f->kd);
    PCP_LOG(PCP_LOGLVL_DEBUG, "Adding flow %p, key_bucket %d",
            f, f->key_bucket);

    for (fdb=ctx->pcp_db.flows + indx; (*fdb) != NULL; fdb=&(*fdb)->next);

    *fdb=f;
    f->next=NULL;
    ctx->pcp_db.flow_cnt++;

    PCP_LOG(PCP_LOGLVL_DEBUG, "total Number of flows added %zu",
            ctx->pcp_db.flow_cnt);

    return PCP_ERR_SUCCESS;
}

pcp_flow_t *pcp_get_flow(struct flow_key_data *fkd, pcp_server_t *s)
{
    pcp_flow_t **fdb;
    uint32_t bucket;
    uint32_t pcp_server_index;

    if ((!fkd) || (!s) || (!s->ctx)) {
        return NULL;
    }
    pcp_server_index=s->index;

    bucket=compute_flow_key(fkd);
    PCP_LOG(PCP_LOGLVL_DEBUG, "Computed key_bucket %d", bucket);
    for (fdb=&s->ctx->pcp_db.flows[bucket]; (*fdb) != NULL; fdb=&(*fdb)->next) {
        if (((*fdb)->pcp_server_indx == pcp_server_index)
                && (0 == memcmp(fkd, &(*fdb)->kd, sizeof(*fkd)))) {
            return *fdb;
        }
    }

    return NULL;
}

pcp_errno pcp_db_rem_flow(pcp_flow_t *f)
{
    pcp_flow_t **fdb=NULL;
    pcp_ctx_t *ctx;

    assert(f && f->ctx);

    if (f->key_bucket == EMPTY) {
        return PCP_ERR_NOT_FOUND;
    }

    ctx=f->ctx;
    PCP_LOG(PCP_LOGLVL_DEBUG, "Removing flow %p, key_bucket %d",
            f, f->key_bucket);

    for (fdb=ctx->pcp_db.flows + f->key_bucket; (*fdb) != NULL;
            fdb=&((*fdb)->next)) {
        if (*fdb == f) {
            (*fdb)->key_bucket=EMPTY;
            (*fdb)=(*fdb)->next;
            ctx->pcp_db.flow_cnt--;
            return PCP_ERR_SUCCESS;
        }
    }

    return PCP_ERR_NOT_FOUND;
}

pcp_errno pcp_db_foreach_flow(pcp_ctx_t *ctx, pcp_db_flow_iterate f, void *data)
{
    pcp_flow_t *fdb, *fdb_next=NULL;
    uint32_t indx;

    assert(f && ctx);

    for (indx=0; indx < FLOW_HASH_SIZE; ++indx) {
        fdb=ctx->pcp_db.flows[indx];
        while (fdb != NULL) {
            fdb_next=(fdb->next);
            if ((*f)(fdb, data)) {
                return PCP_ERR_SUCCESS;
            }
            fdb=fdb_next;
        }

    }

    return PCP_ERR_NOT_FOUND;
}

#ifdef PCP_EXPERIMENTAL
void pcp_db_add_md(pcp_flow_t *f, uint16_t md_id, void *val, size_t val_len)
{
    md_val_t *md;
    uint32_t i;

    assert(f);

    for (i=f->md_val_count, md=f->md_vals; i>0 && md!=NULL; --i, ++md)
    {
        if (md->md_id == md_id) {
            break;
        }
    }
    if (i==0) {
        md = NULL;
    }

    if (!md) {
        md = (md_val_t*) realloc(f->md_vals,
                sizeof(f->md_vals[0])*(f->md_val_count+1));
        if (!md) { //LCOV_EXCL_START
            return;
        } //LCOV_EXCL_STOP
        f->md_vals = md;
        md = f->md_vals + f->md_val_count++;
    }
    md->md_id = md_id;
    if ((val_len>0)&&(val!=NULL)) {
        md->val_len = val_len > sizeof(md->val_buf) ?
            sizeof(md->val_buf) : val_len;
        memcpy(md->val_buf, val, md->val_len);
    } else {
        md->val_len=0;
    }
}
#endif

int pcp_new_server(pcp_ctx_t *ctx, struct in6_addr *ip, uint16_t port, uint32_t scope_id)
{
    uint32_t i;
    pcp_server_t *ret=NULL;

    PCP_LOG_BEGIN(PCP_LOGLVL_DEBUG);

    assert(ctx && ip);

    //initialize array of pcp servers, if not already initialized
    if (!ctx->pcp_db.pcp_servers) {
        ctx->pcp_db.pcp_servers=(pcp_server_t *)calloc(PCP_INIT_SERVER_COUNT,
                sizeof(*ctx->pcp_db.pcp_servers));
        if (!ctx->pcp_db.pcp_servers) {
            char buff[ERR_BUF_LEN];
            pcp_strerror(errno, buff, sizeof(buff));
            PCP_LOG(PCP_LOGLVL_ERR, "Error (%s) occurred during realloc ",
                    buff);
            PCP_LOG_END(PCP_LOGLVL_DEBUG);
            return PCP_ERR_NO_MEM;
        }
        ctx->pcp_db.pcp_servers_length=PCP_INIT_SERVER_COUNT;
    }

    //find first unused record
    for (i=0; i < ctx->pcp_db.pcp_servers_length; ++i) {
        pcp_server_t *s=ctx->pcp_db.pcp_servers + i;
        if (s->server_state == pss_unitialized) {
            ret=s;
            break;
        }
    }

    // if nothing available double the size of array
    if (ret == NULL) {
        ret=(pcp_server_t *)realloc(ctx->pcp_db.pcp_servers,
                sizeof(ctx->pcp_db.pcp_servers[0])
                        * (ctx->pcp_db.pcp_servers_length << 1));
        if (!ret) {
            char buff[ERR_BUF_LEN];

            pcp_strerror(errno, buff, sizeof(buff));
            PCP_LOG(PCP_LOGLVL_ERR, "Error (%s) occurred during realloc ",
                    buff);
            PCP_LOG_END(PCP_LOGLVL_DEBUG);
            return PCP_ERR_NO_MEM;
        }
        ctx->pcp_db.pcp_servers=ret;
        ret=ctx->pcp_db.pcp_servers + ctx->pcp_db.pcp_servers_length;
        memset(ret, 0, sizeof(*ret)*ctx->pcp_db.pcp_servers_length);
        ctx->pcp_db.pcp_servers_length<<=1;
    }

    ret->epoch=~uint32_t(0);
#ifdef PCP_USE_IPV6_SOCKET
    ret->af = AF_INET6;
#else
    ret->af=IN6_IS_ADDR_V4MAPPED(ip) ? AF_INET : AF_INET6;
#endif
    IPV6_ADDR_COPY((struct in6_addr*)ret->pcp_ip, ip);
    ret->pcp_port=port;
    ret->pcp_scope_id=scope_id;
    ret->ctx=ctx;
    ret->server_state=pss_allocated;
    ret->pcp_version=PCP_MAX_SUPPORTED_VERSION;
    createNonce(&ret->nonce);
    ret->index=ret - ctx->pcp_db.pcp_servers;

    PCP_LOG_END(PCP_LOGLVL_DEBUG);
    return ret->index;
}

pcp_server_t *get_pcp_server(pcp_ctx_t *ctx, int pcp_server_index)
{
    PCP_LOG_BEGIN(PCP_LOGLVL_DEBUG);

    assert(ctx);

    if ((pcp_server_index < 0)
            || ((unsigned)pcp_server_index >= ctx->pcp_db.pcp_servers_length)) {

        PCP_LOG(PCP_LOGLVL_WARN, "server index(%d) out of bounds(%zu)",
                pcp_server_index, ctx->pcp_db.pcp_servers_length);
        PCP_LOG_END(PCP_LOGLVL_DEBUG);
        return NULL;
    }

    if (ctx->pcp_db.pcp_servers[pcp_server_index].server_state
            == pss_unitialized) {

        PCP_LOG_END(PCP_LOGLVL_DEBUG);
        return NULL;
    }

    PCP_LOG_END(PCP_LOGLVL_DEBUG);
    return ctx->pcp_db.pcp_servers + pcp_server_index;
}

pcp_errno pcp_db_foreach_server(pcp_ctx_t *ctx, pcp_db_server_iterate f,
        void *data)
{
    uint32_t indx;
    int ret=PCP_ERR_MAX_SIZE;

    PCP_LOG_BEGIN(PCP_LOGLVL_DEBUG);

    assert(ctx && f);

    for (indx=0; indx < ctx->pcp_db.pcp_servers_length; ++indx) {
        if (ctx->pcp_db.pcp_servers[indx].server_state == pss_unitialized) {
            continue;
        }
        if ((*f)(ctx->pcp_db.pcp_servers + indx, data)) {
            ret=PCP_ERR_SUCCESS;
            break;
        }
    } PCP_LOG_END(PCP_LOGLVL_DEBUG);
    return (pcp_errno)ret;
}

typedef struct find_data {
    struct in6_addr *ip;
    pcp_server_t *found_server;
} find_data_t;

static int find_ip(pcp_server_t *s, void *data)
{
    find_data_t *fd=(find_data_t *)data;

    if (IN6_ARE_ADDR_EQUAL(fd->ip, (struct in6_addr *) s->pcp_ip)) {

        fd->found_server=s;
        return 1;
    }
    return 0;
}

pcp_server_t *get_pcp_server_by_ip(pcp_ctx_t *ctx, struct in6_addr *ip)
{
    find_data_t fdata;

    PCP_LOG_BEGIN(PCP_LOGLVL_DEBUG);

    assert(ctx && ip);

    fdata.found_server=NULL;
    fdata.ip=ip;
    pcp_db_foreach_server(ctx, find_ip, &fdata);

    PCP_LOG_END(PCP_LOGLVL_DEBUG);
    return fdata.found_server;
}

void pcp_db_free_pcp_servers(pcp_ctx_t *ctx)
{
    uint32_t i;

    assert(ctx);

    for (i=0; i < ctx->pcp_db.pcp_servers_length; ++i) {
        pcp_server_t *s=ctx->pcp_db.pcp_servers + i;
        pcp_server_state_e state=s->server_state;
        if ((state != pss_unitialized) && (state != pss_allocated)) {
            run_server_state_machine(s, pcpe_terminate);
        }
    }
    free(ctx->pcp_db.pcp_servers);
    ctx->pcp_db.pcp_servers=NULL;
    ctx->pcp_db.pcp_servers_length=0;
}
