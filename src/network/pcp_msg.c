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
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#ifdef WIN32
#include "windows/pcp_win_defines.h"
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif //WIN32
#include "pcp.h"
#include "pcp_utils.h"
#include "pcp_msg.h"
#include "pcp_msg_structs.h"
#include "pcp_logger.h"

static void *add_filter_option(pcp_flow_t *f, void *cur)
{
    pcp_filter_option_t *filter_op=(pcp_filter_option_t *)cur;

    filter_op->option=PCP_OPTION_FILTER;
    filter_op->reserved=0;
    filter_op->len=htons(sizeof(pcp_filter_option_t) - sizeof(pcp_options_hdr_t));
    filter_op->reserved2=0;
    filter_op->filter_prefix=f->filter_prefix;
    filter_op->filter_peer_port=f->filter_port;
    memcpy(&filter_op->filter_peer_ip, &f->filter_ip,
            sizeof(filter_op->filter_peer_ip));
    cur=filter_op->next_data;

    return cur;
}

static void *add_prefer_failure_option(void *cur)
{
    pcp_prefer_fail_option_t *pfailure_op=(pcp_prefer_fail_option_t *)cur;

    pfailure_op->option=PCP_OPTION_PREF_FAIL;
    pfailure_op->reserved=0;
    pfailure_op->len=htons(sizeof(pcp_prefer_fail_option_t) - sizeof(pcp_options_hdr_t));
    cur=pfailure_op->next_data;

    return cur;
}

static void *add_third_party_option(pcp_flow_t *f, void *cur)
{
    pcp_3rd_party_option_t *tp_op=(pcp_3rd_party_option_t *)cur;

    tp_op->option=PCP_OPTION_3RD_PARTY;
    tp_op->reserved=0;
    memcpy(tp_op->ip, &f->third_party_ip, sizeof(f->third_party_ip));
    tp_op->len=htons(sizeof(*tp_op) - sizeof(pcp_options_hdr_t));
    cur=tp_op->next_data;

    return cur;
}

#ifdef PCP_EXPERIMENTAL
static void *add_userid_option(pcp_flow_t *f, void *cur)
{
    pcp_userid_option_t *userid_op = (pcp_userid_option_t *) cur;

    userid_op->option=PCP_OPTION_USERID;
    userid_op->len=htons(sizeof(pcp_userid_option_t) - sizeof(pcp_options_hdr_t));
    memcpy(&(userid_op->userid[0]), &(f->f_userid.userid[0]), MAX_USER_ID);
    cur=userid_op + 1;

    return cur;
}

static void *add_location_option(pcp_flow_t *f, void *cur)
{
    pcp_location_option_t *location_op = (pcp_location_option_t *) cur;

    location_op->option=PCP_OPTION_LOCATION;
    location_op->len=htons(sizeof(pcp_location_option_t) - sizeof(pcp_options_hdr_t));
    memcpy(&(location_op->location[0]), &(f->f_location.location[0]), MAX_GEO_STR);
    cur=location_op + 1;

    return cur;
}

static void *add_deviceid_option(pcp_flow_t *f, void *cur)
{
    pcp_deviceid_option_t *deviceid_op = (pcp_deviceid_option_t *) cur;

    deviceid_op->option=PCP_OPTION_DEVICEID;
    PCP_LOG_BEGIN(PCP_LOGLVL_DEBUG);
    deviceid_op->len=htons(sizeof(pcp_deviceid_option_t) - sizeof(pcp_options_hdr_t));
    memcpy(&(deviceid_op->deviceid[0]), &(f->f_deviceid.deviceid[0]), MAX_DEVICE_ID);
    cur=deviceid_op + 1;

    PCP_LOG_END(PCP_LOGLVL_DEBUG);
    return cur;
}
#endif

#ifdef PCP_FLOW_PRIORITY
static void *add_flowp_option(pcp_flow_t *f, void *cur)
{
    pcp_flow_priority_option_t *flowp_op = (pcp_flow_priority_option_t *)cur;

    flowp_op->option=PCP_OPTION_FLOW_PRIORITY;
    flowp_op->len=htons(sizeof(pcp_flow_priority_option_t) - sizeof(pcp_options_hdr_t));
    flowp_op->dscp_up=f->flowp_dscp_up;
    flowp_op->dscp_down=f->flowp_dscp_down;
    cur=flowp_op->next_data;

    return cur;
}
#endif

#ifdef PCP_EXPERIMENTAL
static inline pcp_metadata_option_t *add_md_option(pcp_flow_t *f,
        pcp_metadata_option_t *md_opt, md_val_t *md)
{
    size_t len_md=md->val_len;
    uint32_t padding=(4 - (len_md % 4)) % 4;
    size_t pcp_msg_len=((const char*)md_opt) - f->pcp_msg_buffer;

    if ( (pcp_msg_len + (sizeof(pcp_metadata_option_t) + len_md + padding)) >
            PCP_MAX_LEN) {
        return md_opt;
    }

    md_opt->option=PCP_OPTION_METADATA;
    md_opt->metadata_id=htonl(md->md_id);
    memcpy(md_opt->metadata, md->val_buf, len_md);
    md_opt->len=htons(sizeof(*md_opt) - sizeof(pcp_options_hdr_t) + len_md + padding);

    return (pcp_metadata_option_t *)(((uint8_t *)(md_opt+1)) + len_md + padding);
}

static void *add_md_options(pcp_flow_t *f, void *cur)
{
    uint32_t i;
    md_val_t *md;
    pcp_metadata_option_t *md_opt=(pcp_metadata_option_t *)cur;

    for (i=f->md_val_count, md=f->md_vals; i>0 && md!=NULL; --i, ++md)
    {
        if (md->val_len) {
            md_opt = add_md_option(f, md_opt, md);
        }
    }
    return md_opt;
}
#endif

static pcp_errno build_pcp_options(pcp_flow_t *flow, void *cur)
{
#ifdef PCP_FLOW_PRIORITY
    if (flow->flowp_option_present) {
        cur=add_flowp_option(flow, cur);
    }
#endif
    if (flow->filter_option_present) {
        cur=add_filter_option(flow, cur);
    }

    if (flow->pfailure_option_present) {
        cur=add_prefer_failure_option(cur);
    }
    if (flow->third_party_option_present) {
        cur=add_third_party_option(flow, cur);
    }
#ifdef PCP_EXPERIMENTAL
    if (flow->f_deviceid.deviceid[0] != '\0') {
        cur=add_deviceid_option(flow, cur);
    }

    if (flow->f_userid.userid[0] != '\0') {
        cur=add_userid_option(flow, cur);
    }

    if (flow->f_location.location[0] != '\0') {
        cur=add_location_option(flow, cur);
    }

    if (flow->md_val_count>0) {
        cur=add_md_options(flow, cur);
    }
#endif

    flow->pcp_msg_len=(uint32_t)(((char*)cur) - flow->pcp_msg_buffer);

    //TODO: implement building all pcp options into msg
    return PCP_ERR_SUCCESS;
}

static pcp_errno build_pcp_peer(pcp_server_t *server, pcp_flow_t *flow,
        void *peer_loc)
{
    void *next=NULL;

    if (server->pcp_version == 1) {
        pcp_peer_v1_t *peer_info=(pcp_peer_v1_t *)peer_loc;

        peer_info->protocol=flow->kd.map_peer.protocol;
        peer_info->int_port=flow->kd.map_peer.src_port;
        peer_info->ext_port=flow->map_peer.ext_port;
        peer_info->peer_port=flow->kd.map_peer.dst_port;
        memcpy(peer_info->ext_ip, &flow->map_peer.ext_ip,
                sizeof(peer_info->ext_ip));
        memcpy(peer_info->peer_ip, &flow->kd.map_peer.dst_ip,
                sizeof(peer_info->peer_ip));
        next=peer_info + 1;
    } else if (server->pcp_version == 2) {
        pcp_peer_v2_t *peer_info=(pcp_peer_v2_t *)peer_loc;

        peer_info->protocol=flow->kd.map_peer.protocol;
        peer_info->int_port=flow->kd.map_peer.src_port;
        peer_info->ext_port=flow->map_peer.ext_port;
        peer_info->peer_port=flow->kd.map_peer.dst_port;
        memcpy(peer_info->ext_ip, &flow->map_peer.ext_ip,
                sizeof(peer_info->ext_ip));
        memcpy(peer_info->peer_ip, &flow->kd.map_peer.dst_ip,
                sizeof(peer_info->peer_ip));
        peer_info->nonce=flow->kd.nonce;
        next=peer_info + 1;
    } else {
        return PCP_ERR_UNSUP_VERSION;
    }
    return build_pcp_options(flow, next);
}

static pcp_errno build_pcp_map(pcp_server_t *server, pcp_flow_t *flow,
        void *map_loc)
{
    void *next=NULL;

    if (server->pcp_version == 1) {
        pcp_map_v1_t *map_info=(pcp_map_v1_t *)map_loc;

        map_info->protocol=flow->kd.map_peer.protocol;
        map_info->int_port=flow->kd.map_peer.src_port;
        map_info->ext_port=flow->map_peer.ext_port;
        memcpy(map_info->ext_ip, &flow->map_peer.ext_ip,
                sizeof(map_info->ext_ip));
        next=map_info + 1;
    } else if (server->pcp_version == 2) {
        pcp_map_v2_t *map_info=(pcp_map_v2_t *)map_loc;

        map_info->protocol=flow->kd.map_peer.protocol;
        map_info->int_port=flow->kd.map_peer.src_port;
        map_info->ext_port=flow->map_peer.ext_port;
        memcpy(map_info->ext_ip, &flow->map_peer.ext_ip,
                sizeof(map_info->ext_ip));
        map_info->nonce=flow->kd.nonce;
        next=map_info + 1;
    } else {
        return PCP_ERR_UNSUP_VERSION;
    }

    return build_pcp_options(flow, next);
}

#ifdef PCP_SADSCP
static pcp_errno build_pcp_sadscp(pcp_server_t *server, pcp_flow_t *flow,
        void *sadscp_loc)
{
    void *next=NULL;

    if (server->pcp_version == 1) {
        return PCP_ERR_UNSUP_VERSION;
    } else if (server->pcp_version == 2) {
        size_t fill_len;
        pcp_sadscp_req_t *sadscp=(pcp_sadscp_req_t *)sadscp_loc;

        sadscp->nonce=flow->kd.nonce;
        sadscp->tolerance_fields=flow->sadscp.toler_fields;

        //app name fill size to multiple of 4
        fill_len=(4-((flow->sadscp.app_name_length+2)%4))%4;

        sadscp->app_name_length=flow->sadscp.app_name_length + fill_len;
        if (flow->sadscp_app_name) {
            memcpy(sadscp->app_name, flow->sadscp_app_name,
                    flow->sadscp.app_name_length);
        } else {
            memset(sadscp->app_name, 0,
                    flow->sadscp.app_name_length);
        }

        next=((uint8_t *)sadscp_loc) + sizeof(pcp_sadscp_req_t) +
                sadscp->app_name_length;
    } else {
        return PCP_ERR_UNSUP_VERSION;
    }

    return build_pcp_options(flow, next);
}
#endif

#ifndef PCP_DISABLE_NATPMP
static pcp_errno build_natpmp_msg(pcp_flow_t *flow)
{
    nat_pmp_announce_req_t *ann_msg;
    nat_pmp_map_req_t *map_info;

    switch (flow->kd.operation) {
        case PCP_OPCODE_ANNOUNCE:
            ann_msg=(nat_pmp_announce_req_t *)flow->pcp_msg_buffer;
            ann_msg->ver=0;
            ann_msg->opcode=NATPMP_OPCODE_ANNOUNCE;
            flow->pcp_msg_len=sizeof(*ann_msg);
            return (pcp_errno)PCP_RES_SUCCESS;

        case PCP_OPCODE_MAP:
            map_info=(nat_pmp_map_req_t *)flow->pcp_msg_buffer;
            switch (flow->kd.map_peer.protocol) {
                case IPPROTO_TCP:
                    map_info->opcode=NATPMP_OPCODE_MAP_TCP;
                    break;
                case IPPROTO_UDP:
                    map_info->opcode=NATPMP_OPCODE_MAP_UDP;
                    break;
                default:
                    return (pcp_errno)PCP_RES_UNSUPP_PROTOCOL;
            }
            map_info->ver=0;
            map_info->lifetime=htonl(flow->lifetime);
            map_info->int_port=flow->kd.map_peer.src_port;
            map_info->ext_port=flow->map_peer.ext_port;
            flow->pcp_msg_len=sizeof(*map_info);
            return (pcp_errno)PCP_RES_SUCCESS;

        default:
            return (pcp_errno)PCP_RES_UNSUPP_OPCODE;
    }
}
#endif

void *build_pcp_msg(pcp_flow_t *flow)
{
    ssize_t ret=-1;
    pcp_server_t *pcp_server=NULL;
    pcp_request_t *req;
    // pointer used for referencing next data structure in linked list
    void *next_data=NULL;

    PCP_LOG_BEGIN(PCP_LOGLVL_DEBUG);

    if (!flow) {
        return NULL;
    }

    pcp_server=get_pcp_server(flow->ctx, flow->pcp_server_indx);

    if (!pcp_server) {
        return NULL;
    }

    if (!flow->pcp_msg_buffer) {
        flow->pcp_msg_buffer=(char*)calloc(1, PCP_MAX_LEN);
        if (flow->pcp_msg_buffer == NULL) {
            PCP_LOG(PCP_LOGLVL_ERR, "%s",
                    "Malloc can't allocate enough memory for the pcp_flow.");
            PCP_LOG_END(PCP_LOGLVL_DEBUG);
            return NULL;
        }
    }

    req=(pcp_request_t *)flow->pcp_msg_buffer;

    if (pcp_server->pcp_version == 0) {
        // NATPMP
#ifndef PCP_DISABLE_NATPMP
        ret=build_natpmp_msg(flow);
#endif
    } else {

        req->ver=pcp_server->pcp_version;

        req->r_opcode|=(uint8_t)(flow->kd.operation & 0x7f); //set  opcode
        req->req_lifetime=htonl((uint32_t)flow->lifetime);

        memcpy(&req->ip, &flow->kd.src_ip, 16);
        // next data in the packet
        next_data=req->next_data;
        flow->pcp_msg_len=(uint32_t)((uint8_t *)next_data - (uint8_t *)req);

        switch (flow->kd.operation) {
            case PCP_OPCODE_PEER:
                ret=build_pcp_peer(pcp_server, flow, next_data);
                break;
            case PCP_OPCODE_MAP:
                ret=build_pcp_map(pcp_server, flow, next_data);
                break;
#ifdef PCP_SADSCP
            case PCP_OPCODE_SADSCP:
                ret=build_pcp_sadscp(pcp_server, flow, next_data);
                break;
#endif
            case PCP_OPCODE_ANNOUNCE:
                ret=0;
                break;
        }
    }

    if (ret < 0) {
        PCP_LOG(PCP_LOGLVL_ERR, "%s", "Unsupported operation.");
        free(flow->pcp_msg_buffer);
        flow->pcp_msg_buffer=NULL;
        flow->pcp_msg_len=0;
        req=NULL;
    }

    PCP_LOG_END(PCP_LOGLVL_DEBUG);
    return req;
}

int validate_pcp_msg(pcp_recv_msg_t *f)
{
    pcp_response_t *resp;

    //check size
    if (((f->pcp_msg_len & 3) != 0) || (f->pcp_msg_len < 4)
            || (f->pcp_msg_len > PCP_MAX_LEN)) {
        PCP_LOG(PCP_LOGLVL_WARN, "Received packet with invalid size %d)",
                f->pcp_msg_len);
        return 0;
    }

    resp=(pcp_response_t *)f->pcp_msg_buffer;
    if ((resp->ver)&&!(resp->r_opcode & 0x80)) {
        PCP_LOG(PCP_LOGLVL_WARN, "%s",
                "Received packet without response bit set");
        return 0;
    }

    if (resp->ver > PCP_MAX_SUPPORTED_VERSION) {
        PCP_LOG(PCP_LOGLVL_WARN,
                "Received PCP msg using unsupported PCP version %d", resp->ver);
        return 0;
    }

    return 1;
}

static pcp_errno parse_options(UNUSED pcp_recv_msg_t *f, UNUSED void *r)
{
    //TODO: implement parsing of pcp options
    return PCP_ERR_SUCCESS;
}

static pcp_errno parse_v1_map(pcp_recv_msg_t *f, void *r)
{
    pcp_map_v1_t *m;
    size_t rest_size=f->pcp_msg_len - (((char*)r) - f->pcp_msg_buffer);

    if (rest_size < sizeof(pcp_map_v1_t)) {
        return PCP_ERR_RECV_FAILED;
    }

    m=(pcp_map_v1_t *)r;
    f->kd.map_peer.src_port=m->int_port;
    f->kd.map_peer.protocol=m->protocol;
    f->assigned_ext_port=m->ext_port;
    memcpy(&f->assigned_ext_ip, m->ext_ip, sizeof(f->assigned_ext_ip));

    if (rest_size > sizeof(pcp_map_v1_t)) {
        return parse_options(f, m + 1);
    }
    return PCP_ERR_SUCCESS;
}

static pcp_errno parse_v2_map(pcp_recv_msg_t *f, void *r)
{
    pcp_map_v2_t *m;
    size_t rest_size=f->pcp_msg_len - (((char*)r) - f->pcp_msg_buffer);

    if (rest_size < sizeof(pcp_map_v2_t)) {
        return PCP_ERR_RECV_FAILED;
    }

    m=(pcp_map_v2_t *)r;
    f->kd.nonce=m->nonce;
    f->kd.map_peer.src_port=m->int_port;
    f->kd.map_peer.protocol=m->protocol;
    f->assigned_ext_port=m->ext_port;
    memcpy(&f->assigned_ext_ip, m->ext_ip, sizeof(f->assigned_ext_ip));

    if (rest_size > sizeof(pcp_map_v2_t)) {
        return parse_options(f, m + 1);
    }
    return PCP_ERR_SUCCESS;
}

static pcp_errno parse_v1_peer(pcp_recv_msg_t *f, void *r)
{
    pcp_peer_v1_t *m;
    size_t rest_size=f->pcp_msg_len - (((char*)r) - f->pcp_msg_buffer);

    if (rest_size < sizeof(pcp_peer_v1_t)) {
        return PCP_ERR_RECV_FAILED;
    }

    m=(pcp_peer_v1_t *)r;
    f->kd.map_peer.src_port=m->int_port;
    f->kd.map_peer.protocol=m->protocol;
    f->kd.map_peer.dst_port=m->peer_port;
    f->assigned_ext_port=m->ext_port;
    memcpy(&f->kd.map_peer.dst_ip, m->peer_ip, sizeof(f->kd.map_peer.dst_ip));
    memcpy(&f->assigned_ext_ip, m->ext_ip, sizeof(f->assigned_ext_ip));

    if (rest_size > sizeof(pcp_peer_v1_t)) {
        return parse_options(f, m + 1);
    }
    return PCP_ERR_SUCCESS;
}

static pcp_errno parse_v2_peer(pcp_recv_msg_t *f, void *r)
{
    pcp_peer_v2_t *m;
    size_t rest_size=f->pcp_msg_len - (((char*)r) - f->pcp_msg_buffer);

    if (rest_size < sizeof(pcp_peer_v2_t)) {
        return PCP_ERR_RECV_FAILED;
    }

    m=(pcp_peer_v2_t *)r;
    f->kd.nonce=m->nonce;
    f->kd.map_peer.src_port=m->int_port;
    f->kd.map_peer.protocol=m->protocol;
    f->kd.map_peer.dst_port=m->peer_port;
    f->assigned_ext_port=m->ext_port;
    memcpy(&f->kd.map_peer.dst_ip, m->peer_ip, sizeof(f->kd.map_peer.dst_ip));
    memcpy(&f->assigned_ext_ip, m->ext_ip, sizeof(f->assigned_ext_ip));

    if (rest_size > sizeof(pcp_peer_v2_t)) {
        return parse_options(f, m + 1);
    }
    return PCP_ERR_SUCCESS;
}

#ifdef PCP_SADSCP
static pcp_errno parse_sadscp(pcp_recv_msg_t *f, void *r)
{
    pcp_sadscp_resp_t *d;
    size_t rest_size = f->pcp_msg_len - (((char*) r) - f->pcp_msg_buffer);

    if (rest_size < sizeof(pcp_sadscp_resp_t)) {
        return PCP_ERR_RECV_FAILED;
    }
    d = (pcp_sadscp_resp_t *) r;
    f->kd.nonce = d->nonce;
    f->recv_dscp = d->a_r_dscp & (0x3f); //mask 6 lower bits

    return PCP_ERR_SUCCESS;
}
#endif

#ifndef PCP_DISABLE_NATPMP
static pcp_errno parse_v0_resp(pcp_recv_msg_t *f, pcp_response_t *resp)
{
    switch(f->kd.operation) {
        case PCP_OPCODE_ANNOUNCE:
            if (f->pcp_msg_len == sizeof(nat_pmp_announce_resp_t)) {
                nat_pmp_announce_resp_t *r=(nat_pmp_announce_resp_t *)resp;

                f->recv_epoch=ntohl(r->epoch);
                S6_ADDR32(&f->assigned_ext_ip)[0]=0;
                S6_ADDR32(&f->assigned_ext_ip)[1]=0;
                S6_ADDR32(&f->assigned_ext_ip)[2]=htonl(0xFFFF);
                S6_ADDR32(&f->assigned_ext_ip)[3]=r->ext_ip;

                return PCP_ERR_SUCCESS;
            }
            break;
        case NATPMP_OPCODE_MAP_TCP:
        case NATPMP_OPCODE_MAP_UDP:
            if (f->pcp_msg_len == sizeof(nat_pmp_map_resp_t)) {
                nat_pmp_map_resp_t *r=(nat_pmp_map_resp_t *)resp;

                f->assigned_ext_port=r->ext_port;
                f->kd.map_peer.src_port=r->int_port;
                f->recv_epoch=ntohl(r->epoch);
                f->recv_lifetime=ntohl(r->lifetime);
                f->recv_result=ntohs(r->result);
                f->kd.map_peer.protocol=
                        f->kd.operation == (uint8_t)(NATPMP_OPCODE_MAP_TCP ?
                                IPPROTO_TCP : IPPROTO_UDP);
                f->kd.operation=PCP_OPCODE_MAP;
                return PCP_ERR_SUCCESS;
            }
            break;
        default:
            break;
    }

    if (f->pcp_msg_len == sizeof(nat_pmp_inv_version_resp_t)) {
        nat_pmp_inv_version_resp_t *r=(nat_pmp_inv_version_resp_t *)resp;

        f->recv_result=ntohs(r->result);
        f->recv_epoch=ntohl(r->epoch);
        return PCP_ERR_SUCCESS;
    }

    return PCP_ERR_RECV_FAILED;
}
#endif

static pcp_errno parse_v1_resp(pcp_recv_msg_t *f, pcp_response_t *resp)
{
    if (f->pcp_msg_len < sizeof(pcp_response_t)) {
        return PCP_ERR_RECV_FAILED;
    }

    f->recv_lifetime=ntohl(resp->lifetime);
    f->recv_epoch=ntohl(resp->epochtime);

    switch (f->kd.operation) {
        case PCP_OPCODE_ANNOUNCE:
            return PCP_ERR_SUCCESS;
        case PCP_OPCODE_MAP:
            return parse_v1_map(f, resp->next_data);
        case PCP_OPCODE_PEER:
            return parse_v1_peer(f, resp->next_data);
        default:
            return PCP_ERR_RECV_FAILED;
    }
}

static pcp_errno parse_v2_resp(pcp_recv_msg_t *f, pcp_response_t *resp)
{
    if (f->pcp_msg_len < sizeof(pcp_response_t)) {
        return PCP_ERR_RECV_FAILED;
    }

    f->recv_lifetime=ntohl(resp->lifetime);
    f->recv_epoch=ntohl(resp->epochtime);

    switch (f->kd.operation) {
        case PCP_OPCODE_ANNOUNCE:
            return PCP_ERR_SUCCESS;
        case PCP_OPCODE_MAP:
            return parse_v2_map(f, resp->next_data);
        case PCP_OPCODE_PEER:
            return parse_v2_peer(f, resp->next_data);
#ifdef PCP_SADSCP
        case PCP_OPCODE_SADSCP:
            return parse_sadscp(f, resp->next_data);
#endif
        default:
            return PCP_ERR_RECV_FAILED;
    }
}

pcp_errno parse_response(pcp_recv_msg_t *f)
{
    pcp_response_t *resp=(pcp_response_t *)f->pcp_msg_buffer;

    f->recv_version=resp->ver;
    f->recv_result=resp->result_code;
    memset(&f->kd, 0, sizeof(f->kd));

    f->kd.operation=resp->r_opcode & 0x7f;

    PCP_LOG(PCP_LOGLVL_DEBUG, "parse_response: version: %d", f->recv_version);
    PCP_LOG(PCP_LOGLVL_DEBUG, "parse_response: result: %d", f->recv_result);

    switch (f->recv_version) {
#ifndef PCP_DISABLE_NATPMP
        case 0:
            return parse_v0_resp(f, resp);
            break;
#endif
        case 1:
            return parse_v1_resp(f, resp);
            break;
        case 2:
            return parse_v2_resp(f, resp);
            break;
    }
    return PCP_ERR_UNSUP_VERSION;
}

