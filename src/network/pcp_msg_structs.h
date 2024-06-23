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

#ifndef PCP_MSG_STRUCTS_H_
#define PCP_MSG_STRUCTS_H_

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable:4200)
#endif // _MSC_VER
#define PCP_MAX_LEN          1100
#define PCP_OPCODE_ANNOUNCE     0
#define PCP_OPCODE_MAP          1
#define PCP_OPCODE_PEER         2
#define PCP_OPCODE_SADSCP       3
#define NATPMP_OPCODE_ANNOUNCE  0
#define NATPMP_OPCODE_MAP_UDP   1
#define NATPMP_OPCODE_MAP_TCP   2

/* Possible response codes sent by server, as a result of client request*/
#define PCP_RES_SUCCESS                   0
#define PCP_RES_UNSUPP_VERSION            1
#define PCP_RES_NOT_AUTHORIZED            2
#define PCP_RES_MALFORMED_REQUEST         3
#define PCP_RES_UNSUPP_OPCODE             4
#define PCP_RES_UNSUPP_OPTION             5
#define PCP_RES_MALFORMED_OPTION          6
#define PCP_RES_NETWORK_FAILURE           7
#define PCP_RES_NO_RESOURCES              8
#define PCP_RES_UNSUPP_PROTOCOL           9
#define PCP_RES_USER_EX_QUOTA             10
#define PCP_RES_CANNOT_PROVIDE_EXTERNAL   11
#define PCP_RES_ADDRESS_MISMATCH          12
#define PCP_RES_EXCESSIVE_REMOTE_PEERS    13

typedef enum pcp_options {
    PCP_OPTION_3RD_PARTY=1,
    PCP_OPTION_PREF_FAIL=2,
    PCP_OPTION_FILTER=3,
    PCP_OPTION_DEVICEID=96, /*private range */
    PCP_OPTION_LOCATION=97,
    PCP_OPTION_USERID=98,
    PCP_OPTION_FLOW_PRIORITY=99,
    PCP_OPTION_METADATA=100
} pcp_options_t;

#pragma pack(push,1)

#ifndef MAX_USER_ID
#define MAX_USER_ID 512
#endif
#ifndef MAX_DEViCE_ID_STR
#define MAX_DEVICE_ID 32
#endif
#ifndef MAX_GEO_STR
#define MAX_GEO_STR 32
#endif

/* PCP common request header*/
typedef struct pcp_request {
    uint8_t ver;
    uint8_t r_opcode;
    uint16_t reserved;
    uint32_t req_lifetime;
    uint32_t ip[4]; /* ipv4 will be represented
     by the ipv4 mapped ipv6 */
    uint8_t next_data[0];
} pcp_request_t;

/* PCP common response header*/
typedef struct pcp_response {
    uint8_t ver;
    /* R indicates Request (0) or Response (1)
       Opcode is 7 bit value specifying operation MAP or PEER */
    uint8_t r_opcode;
    /* reserved bits, must be 0 on transmission and must be ignored on
       reception */
    uint8_t reserved;
    uint8_t result_code;
    /* an unsigned 32-bit integer, in seconds {0, 2^32-1}*/
    uint32_t lifetime;
    /* epoch indicates how long has PCP server had its current mappings
       it increases by 1 every second */
    uint32_t epochtime;
    /* For requests that were successfully parsed this must be sent as 0 */
    uint32_t reserved1[3];
    uint8_t next_data[0];
} pcp_response_t;

typedef struct pcp_options_hdr {
    /* Most significant bit indicates if this option is mandatory (0)
       or optional (1) */
    uint8_t code;
    /* MUST be set to 0 on transmission and MUST be ignored on reception */
    uint8_t reserved;
    /* indicates the length of the enclosed data in octets (see RFC) */
    uint16_t len;
    uint8_t next_data[0];
} pcp_options_hdr_t;

typedef struct nat_pmp_announce_req {
    uint8_t ver;
    uint8_t opcode;
} nat_pmp_announce_req_t;

typedef struct nat_pmp_announce_resp {
    uint8_t ver;
    uint8_t opcode;
    uint16_t result;
    uint32_t epoch;
    uint32_t ext_ip;
} nat_pmp_announce_resp_t;

typedef struct nat_pmp_map_req {
    uint8_t ver;
    uint8_t opcode;
    uint16_t reserved;
    uint16_t int_port;
    uint16_t ext_port;
    uint32_t lifetime;
} nat_pmp_map_req_t;

typedef struct nat_pmp_map_resp {
    uint8_t ver;
    uint8_t opcode;
    uint16_t result;
    uint32_t epoch;
    uint16_t int_port;
    uint16_t ext_port;
    uint32_t lifetime;
} nat_pmp_map_resp_t;

typedef struct nat_pmp_inv_version_resp {
    uint8_t ver;
    uint8_t opcode;
    uint16_t result;
    uint32_t epoch;
} nat_pmp_inv_version_resp_t;

struct pcp_nonce {
    uint32_t n[3];
};

/* same for both request and response */
typedef struct pcp_map_v2 {
    struct pcp_nonce nonce;
    uint8_t protocol;
    uint8_t reserved[3];
    uint16_t int_port;
    uint16_t ext_port;
    uint32_t ext_ip[4]; /* ipv4 will be represented by the ipv4 mapped ipv6 */
    uint8_t next_data[0];
} pcp_map_v2_t;

/* same for both request and response */
typedef struct pcp_map_v1 {
    uint8_t protocol;
    uint8_t reserved[3];
    uint16_t int_port;
    uint16_t ext_port;
    uint32_t ext_ip[4]; /* ipv4 will be represented by the ipv4 mapped ipv6 */
    uint8_t next_data[0];
} pcp_map_v1_t;

/* same for both request and response */
typedef struct pcp_peer_v1 {
    uint8_t protocol;
    uint8_t reserved[3];
    uint16_t int_port;
    uint16_t ext_port;
    uint32_t ext_ip[4]; /* ipv4 will be represented by the ipv4 mapped ipv6 */
    uint16_t peer_port;
    uint16_t reserved1;
    uint32_t peer_ip[4];
    uint8_t next_data[0];
} pcp_peer_v1_t;

/* same for both request and response */
typedef struct pcp_peer_v2 {
    struct pcp_nonce nonce;
    uint8_t protocol;
    uint8_t reserved[3];
    uint16_t int_port;
    uint16_t ext_port;
    uint32_t ext_ip[4]; /* ipv4 will be represented by the ipv4 mapped ipv6 */
    uint16_t peer_port;
    uint16_t reserved1;
    uint32_t peer_ip[4];
    uint8_t next_data[0];
} pcp_peer_v2_t;

typedef struct pcp_sadscp_req {
    struct pcp_nonce nonce;
    uint8_t tolerance_fields;
    uint8_t app_name_length;
    char app_name[0];
} pcp_sadscp_req_t;

typedef struct pcp_sadscp_resp {
    struct pcp_nonce nonce;
    uint8_t a_r_dscp;
    uint8_t reserved[3];
} pcp_sadscp_resp_t;

typedef struct pcp_location_option {
    uint8_t option;
    uint8_t reserved;
    uint16_t len;
    //float   latitude;
    //float   longitude;
    char location[MAX_GEO_STR];
} pcp_location_option_t;

typedef struct pcp_userid_option {
    uint8_t option;
    uint8_t reserved;
    uint16_t len;
    char userid[MAX_USER_ID];
} pcp_userid_option_t;

typedef struct pcp_deviceid_option {
    uint8_t option;
    uint8_t reserved;
    uint16_t len;
    // uint32_t device_class;
    char deviceid[MAX_DEVICE_ID];
} pcp_deviceid_option_t;

#define FOREACH_DEVICE(DEVICE) \
        DEVICE(smartphone)  \
        DEVICE(iphone)   \
        DEVICE(unknown)

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING ,

typedef enum DEVICE_ENUM {
    FOREACH_DEVICE(GENERATE_ENUM)
} device_enum_e;

typedef struct pcp_prefer_fail_option {
    uint8_t option;
    uint8_t reserved;
    uint16_t len;
    uint8_t next_data[0];
} pcp_prefer_fail_option_t;

typedef struct pcp_3rd_party_option {
    uint8_t option;
    uint8_t reserved;
    uint16_t len;
    uint32_t ip[4];
    uint8_t next_data[0];
} pcp_3rd_party_option_t;

typedef struct pcp_filter_option {
    uint8_t option;
    uint8_t reserved;
    uint16_t len;
    uint8_t reserved2;
    uint8_t filter_prefix;
    uint16_t filter_peer_port;
    uint32_t filter_peer_ip[4];
    uint8_t next_data[0];
} pcp_filter_option_t;

typedef struct pcp_flow_priority_option {
    uint8_t option;
    uint8_t reserved;
    uint16_t len;
    uint8_t dscp_up;
    uint8_t dscp_down;
#define PCP_DSCP_MASK ((1<<6)-1)
    uint8_t reserved2;
    /* most significant bit is used for response */
    uint8_t response_bit;
//#define PCP_FLOW_OPTION_RESP_P (1<<7)
    uint8_t next_data[0];
} pcp_flow_priority_option_t;

typedef struct pcp_metadata_option {
    uint8_t option;
    uint8_t reserved;
    uint16_t len;
    uint32_t metadata_id;
    uint8_t metadata[];
} pcp_metadata_option_t;

#pragma pack(pop)

#ifdef _MSC_VER
#pragma warning (pop)
#endif // _MSC_VER
#endif /* PCP_MSG_STRUCTS_H_ */
