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

#ifndef PCP_EVENT_HANDLER_H_
#define PCP_EVENT_HANDLER_H_

#include "pcp_msg_structs.h"

typedef enum {
    pfs_any                         = -1,
    pfs_idle                        = 0,
    pfs_wait_for_server_init        = 1,
    pfs_send                        = 2,
    pfs_wait_resp                   = 3,
    pfs_wait_after_short_life_error = 4,
    pfs_wait_for_lifetime_renew     = 5,
    pfs_send_renew                  = 6,
    pfs_failed                      = 7
} pcp_flow_state_e;

typedef enum {
    pcpe_any, pcpe_timeout, pcpe_io_event, pcpe_terminate
} pcp_event_e;

typedef enum {
    fev_flow_timedout,
    fev_server_initialized,
    fev_send,
    fev_msg_sent,
    fev_failed,
    fev_none,
    fev_server_restarted,
    fev_ignored,
    FEV_RES_BEGIN,
    fev_res_success           = FEV_RES_BEGIN + PCP_RES_SUCCESS,
    fev_res_unsupp_version    = FEV_RES_BEGIN + PCP_RES_UNSUPP_VERSION,
    fev_res_not_authorized    = FEV_RES_BEGIN + PCP_RES_NOT_AUTHORIZED,
    fev_res_malformed_request = FEV_RES_BEGIN + PCP_RES_MALFORMED_REQUEST,
    fev_res_unsupp_opcode     = FEV_RES_BEGIN + PCP_RES_UNSUPP_OPCODE,
    fev_res_unsupp_option     = FEV_RES_BEGIN + PCP_RES_UNSUPP_OPTION,
    fev_res_malformed_option  = FEV_RES_BEGIN + PCP_RES_MALFORMED_OPTION,
    fev_res_network_failure   = FEV_RES_BEGIN + PCP_RES_NETWORK_FAILURE,
    fev_res_no_resources      = FEV_RES_BEGIN + PCP_RES_NO_RESOURCES,
    fev_res_unsupp_protocol   = FEV_RES_BEGIN + PCP_RES_UNSUPP_PROTOCOL,
    fev_res_user_ex_quota     = FEV_RES_BEGIN + PCP_RES_USER_EX_QUOTA,
    fev_res_cant_provide_ext  = FEV_RES_BEGIN + PCP_RES_CANNOT_PROVIDE_EXTERNAL,
    fev_res_address_mismatch  = FEV_RES_BEGIN + PCP_RES_ADDRESS_MISMATCH,
    fev_res_exc_remote_peers  = FEV_RES_BEGIN + PCP_RES_EXCESSIVE_REMOTE_PEERS,
} pcp_flow_event_e;

typedef enum {
    pss_any=-1,
    pss_unitialized,
    pss_allocated,
    pss_ping,
    pss_wait_ping_resp,
    pss_version_negotiation,
    pss_send_all_msgs,
    pss_wait_io,
    pss_wait_io_calc_nearest_timeout,
    pss_server_restart,
    pss_server_reping,
    pss_set_not_working,
    pss_not_working,
    PSS_COUNT
} pcp_server_state_e;

void pcp_flow_updated(pcp_flow_t *f);

typedef struct pcp_server pcp_server_t;

pcp_errno run_server_state_machine(pcp_server_t *s, pcp_event_e event);

void pcp_fd_change_notify(pcp_server_t *s, int added);

#endif /* PCP_EVENT_HANDLER_H_ */
