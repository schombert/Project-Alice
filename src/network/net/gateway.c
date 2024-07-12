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

#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#ifndef WIN32
#include <sys/socket.h>         // place it before <net/if.h> struct sockaddr
#endif //WIN32
#ifdef __linux__
#define USE_NETLINK
#include <linux/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef WIN32
#include <sys/ioctl.h>          // ioctl()
#include <net/if.h>             //struct ifreq
#endif //WIN32
#ifdef WIN32
#undef USE_NETLINK
#undef USE_SOCKET_ROUTE
#define USE_WIN32_CODE

#include <winsock2.h>
#include <ws2ipdef.h>
#include <Iphlpapi.h>
#include <ws2tcpip.h>
#include "windows/pcp_win_defines.h"
#endif

#if defined(__APPLE__) || defined(__FreeBSD__)
#include <sys/sysctl.h>
#include <net/if_dl.h>          //struct sockaddr_dl
#define USE_SOCKET_ROUTE
#endif

#ifndef WIN32
#include <arpa/inet.h>          // inet_addr()
#include <net/route.h>          // struct rt_msghdr
#ifdef USE_SOCKET_ROUTE
#include <ifaddrs.h>            //getifaddrs() freeifaddrs()
#endif
#include <net/if.h>
#include <net/route.h>
#include <netinet/in.h>         //IPPROTO_GRE sturct sockaddr_in INADDR_ANY
#endif

#if defined(BSD) || defined(__FreeBSD_kernel__)
#define USE_SOCKET_ROUTE
#undef USE_WIN32_CODE
#endif

#if (defined(sun) && defined(__SVR4))
#define USE_SOCKET_ROUTE
#undef USE_WIN32_CODE
#endif

#include "gateway.h"
#include "pcp_logger.h"
#include "unp.h"
#include "pcp_utils.h"

#ifndef WIN32
#define SUCCESS (0)
#define FAILED  (-1)
#define USE_WIN32_CODE
#endif

#define TO_IPV6MAPPED(x)        S6_ADDR32(x)[3] = S6_ADDR32(x)[0];\
                                S6_ADDR32(x)[0] = 0;\
                                S6_ADDR32(x)[1] = 0;\
                                S6_ADDR32(x)[2] = htonl(0xFFFF);

#ifdef USE_NETLINK

#define BUFSIZE 8192

static ssize_t readNlSock(int sockFd, char *bufPtr, unsigned seqNum,
        unsigned pId)
{
    struct nlmsghdr *nlHdr;
    ssize_t readLen=0, msgLen=0;

    do {
        /* Receive response from the kernel */
        readLen=recv(sockFd, bufPtr, BUFSIZE - msgLen, 0);
        if (readLen == -1) {
            char errmsg[128];
            pcp_strerror(errno, errmsg, sizeof(errmsg));
            PCP_LOG(PCP_LOGLVL_DEBUG, "SOCK READ: %s", errmsg);
            return -1;
        }

        nlHdr=(struct nlmsghdr *)bufPtr;

        /* Check if the header is valid */
        if ((NLMSG_OK(nlHdr, (unsigned)readLen) == 0)
                || (nlHdr->nlmsg_type == NLMSG_ERROR)) {
            PCP_LOG(PCP_LOGLVL_DEBUG, "%s", "Error in received packet");
            return -1;
        }

        /* Check if the its the last message */
        if (nlHdr->nlmsg_type == NLMSG_DONE) {
            break;
        } else {
            /* Else move the pointer to buffer appropriately */
            bufPtr+=readLen;
            msgLen+=readLen;
        }

        /* Check if its a multi part message */
        if ((nlHdr->nlmsg_flags & NLM_F_MULTI) == 0) {
            /* return if its not */
            break;
        }
    } while ((nlHdr->nlmsg_seq != seqNum) || (nlHdr->nlmsg_pid != pId));
    return msgLen;
}

int getgateways(struct sockaddr_in6 **gws)
{
    struct nlmsghdr *nlMsg;
    char msgBuf[BUFSIZE];

    int sock, msgSeq=0;
    ssize_t len;
    int ret;

    if (!gws) {
        return PCP_ERR_BAD_ARGS;
    }

    /* Create Socket */
    sock=socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE);
    if (sock < 0) {
        PCP_LOG(PCP_LOGLVL_DEBUG, "%s", "Netlink Socket Creation Failed...");
        return PCP_ERR_UNKNOWN;
    }

    /* Initialize the buffer */
    memset(msgBuf, 0, BUFSIZE);

    /* point the header and the msg structure pointers into the buffer */
    nlMsg=(struct nlmsghdr *)msgBuf;

    /* Fill in the nlmsg header*/
    nlMsg->nlmsg_len=NLMSG_LENGTH(sizeof(struct rtmsg)); // Length of message.
    nlMsg->nlmsg_type=RTM_GETROUTE; // Get the routes from kernel routing table.

    nlMsg->nlmsg_flags=NLM_F_DUMP | NLM_F_REQUEST; // The message is a request for dump.
    nlMsg->nlmsg_seq=msgSeq++; // Sequence of the message packet.
    nlMsg->nlmsg_pid=getpid(); // PID of process sending the request.

    /* Send the request */
    len=send(sock, nlMsg, nlMsg->nlmsg_len, 0);
    if (len == -1) {
        PCP_LOG(PCP_LOGLVL_DEBUG, "%s", "Write To Netlink Socket Failed...");
        ret=PCP_ERR_SEND_FAILED;
        goto end;
    }

    /* Read the response */
    len=readNlSock(sock, msgBuf, msgSeq, getpid());
    if (len < 0) {
        PCP_LOG(PCP_LOGLVL_DEBUG, "%s", "Read From Netlink Socket Failed...");
        ret=PCP_ERR_RECV_FAILED;
        goto end;
    }
    /* Parse and print the response */
    ret=0;

    for (; NLMSG_OK(nlMsg,(unsigned)len); nlMsg=NLMSG_NEXT(nlMsg,len)) {
        struct rtmsg *rtMsg;
        struct rtattr *rtAttr;
        int rtLen;
        unsigned int scope_id = 0;
        struct in6_addr addr;
        int found = 0;
        rtMsg=(struct rtmsg *)NLMSG_DATA(nlMsg);

        /* If the route is not for AF_INET(6) or does not belong to main
           routing table then return. */
        if (((rtMsg->rtm_family != AF_INET) && (rtMsg->rtm_family != AF_INET6))
                || (rtMsg->rtm_table != RT_TABLE_MAIN)) {
            continue;
        }

        /* get the rtattr field */
        rtAttr=(struct rtattr *)RTM_RTA(rtMsg);
        rtLen=RTM_PAYLOAD(nlMsg);
        for (; RTA_OK(rtAttr,rtLen); rtAttr=RTA_NEXT(rtAttr,rtLen)) {
            size_t rtaLen=RTA_PAYLOAD(rtAttr);
            if (rtaLen > sizeof(struct in6_addr)) {
                continue;
            }
            if (rtAttr->rta_type == RTA_OIF) {
                memcpy(&scope_id, RTA_DATA(rtAttr), sizeof(unsigned int));
            } else if (rtAttr->rta_type == RTA_GATEWAY) {
                memset(&addr, 0, sizeof(struct in6_addr));
                memcpy(&addr, RTA_DATA(rtAttr), rtaLen);
                if (rtMsg->rtm_family == AF_INET) {
                    TO_IPV6MAPPED(&addr);
                }
                found=1;
            }
        }
        if (found) {
            struct sockaddr_in6 *tmp_gws;
            tmp_gws=(struct sockaddr_in6 *)realloc(*gws,
                    sizeof(struct sockaddr_in6) * (ret + 1));
            if (!tmp_gws) {
                PCP_LOG(PCP_LOGLVL_ERR, "%s",
                        "Error allocating memory");
                if (*gws) {
                    free(*gws);
                    *gws=NULL;
                }
                ret=PCP_ERR_NO_MEM;
                goto end;
            }
            *gws=tmp_gws;
            (*gws + ret)->sin6_family=AF_INET6;
            memcpy(&((*gws + ret)->sin6_addr), &addr, sizeof(addr));
            (*gws + ret)->sin6_scope_id=scope_id;
            SET_SA_LEN(*gws + ret, sizeof(struct sockaddr_in6))
            ret++;
        }
    }
end:
    if (close(sock)) {
        PCP_LOG(PCP_LOGLVL_DEBUG, "%s", "Close socket error");
    }
    return ret;
}

#endif /* #ifdef USE_NETLINK */

#if defined (USE_WIN32_CODE) && defined(WIN32)

#if 0 // WINVER>=NTDDI_VISTA
int getgateways(struct in6_addr **gws)
{
    PMIB_IPFORWARD_TABLE2 ipf_table;
    unsigned int i;

    if (!gws) {
        return PCP_ERR_UNKNOWN;
    }

    if (GetIpForwardTable2(AF_UNSPEC, &ipf_table) != NO_ERROR) {
        return PCP_ERR_UNKNOWN;
    }

    if (!ipf_table) {
        return PCP_ERR_UNKNOWN;
    }

    *gws=(struct in6_addr *)calloc(ipf_table->NumEntries,
            sizeof(struct in6_addr));
    if (*gws) {
        PCP_LOG(PCP_LOGLVL_DEBUG, "%s", "Error allocating memory");
        return PCP_ERR_NO_MEM;
    }

    for (i=0; i < ipf_table->NumEntries; ++i) {
        if (ipf_table->Table[i].NextHop.si_family == AF_INET) {
            S6_ADDR32((*gws)+i)[0]=
                    ipf_table->Table[i].NextHop.Ipv4.sin_addr.s_addr;
            TO_IPV6MAPPED(((*gws)+i));
        }
        if (ipf_table->Table[i].NextHop.si_family == AF_INET6) {
            memcpy((*gws) + i, &ipf_table->Table[i].NextHop.Ipv6.sin6_addr,
                    sizeof(struct in6_addr));
        }
    }
    return i;
}
#else
int getgateways(struct sockaddr_in6 **gws)
{
    PMIB_IPFORWARDTABLE ipf_table;
    DWORD ipft_size=0;
    int i, ret;

    if (!gws) {
        return PCP_ERR_UNKNOWN;
    }

    ipf_table=(MIB_IPFORWARDTABLE *)malloc(sizeof(MIB_IPFORWARDTABLE));
    if (!ipf_table) {
        PCP_LOG(PCP_LOGLVL_DEBUG, "%s", "Error allocating memory");
        ret=PCP_ERR_NO_MEM;
        goto end;
    }

    if (GetIpForwardTable(ipf_table, &ipft_size, 0)
            == ERROR_INSUFFICIENT_BUFFER) {
        MIB_IPFORWARDTABLE *new_ipf_table;
        new_ipf_table=(MIB_IPFORWARDTABLE *)realloc(ipf_table, ipft_size);
        if (!new_ipf_table) {
            PCP_LOG(PCP_LOGLVL_DEBUG, "%s", "Error allocating memory");
            ret=PCP_ERR_NO_MEM;
            goto end;
        }
        ipf_table=new_ipf_table;
    }

    if (GetIpForwardTable(ipf_table, &ipft_size, 0) != NO_ERROR) {
        PCP_LOG(PCP_LOGLVL_DEBUG, "%s", "GetIpForwardTable failed.");
        ret=PCP_ERR_UNKNOWN;
        goto end;
    }

    *gws=(struct sockaddr_in6 *)calloc(ipf_table->dwNumEntries,
            sizeof(struct sockaddr_in6));
    if (!*gws) {
        PCP_LOG(PCP_LOGLVL_DEBUG, "%s", "Error allocating memory");
        ret=PCP_ERR_NO_MEM;
        goto end;
    }

    for (ret=0, i=0; i < (int)ipf_table->dwNumEntries; i++) {
        if (ipf_table->table[i].dwForwardType == MIB_IPROUTE_TYPE_INDIRECT) {
            (*gws)[ret].sin6_family = AF_INET6;
            S6_ADDR32(&(*gws)[ret].sin6_addr)[0]=
                (uint32_t)ipf_table->table[i].dwForwardNextHop;
            TO_IPV6MAPPED(&(*gws)[ret].sin6_addr);
            ret++;
        }
    }
end:
    if (ipf_table)
        free(ipf_table);

    return ret;
}
#endif

#endif /* #ifdef USE_WIN32_CODE */

#ifdef USE_SOCKET_ROUTE

struct sockaddr;
struct in6_addr;

/* Adapted from Richard Stevens, UNIX Network Programming  */

/*
 * Round up 'a' to next multiple of 'size', which must be a power of 2
 */
#define ROUNDUP(a, size) (((a) & ((size)-1)) ? (1 + ((a) | ((size)-1))) : (a))

/*
 * Step to next socket address structure;
 * if sa_len is 0, assume it is sizeof(u_long). Using u_long only works on 32-bit
 machines. In 64-bit machines it needs to be u_int32_t !!
 */
#define NEXT_SA(ap)    ap = (struct sockaddr *) \
    ((caddr_t) ap + (ap->sa_len ? ROUNDUP(ap->sa_len, sizeof(uint32_t)) : \
                                    sizeof(uint32_t)))

/* thanks Stevens for this very handy function */
static void get_rtaddrs(int addrs, struct sockaddr *sa,
        struct sockaddr **rti_info)
{
    int i;

    for (i=0; i < RTAX_MAX; i++) {
        if (addrs & (1 << i)) {
            rti_info[i]=sa;
            NEXT_SA(sa);
        } else
            rti_info[i]=NULL;
    }
}

/* Portable (hopefully) function to lookup routing tables. sysctl()'s
 advantage is that it does not need root permissions. Routing sockets
 need root permission since it is of type SOCK_RAW. */
static char *
net_rt_dump(int type, int family, int flags, size_t *lenp)
{
    int mib[6];
    char *buf;

    mib[0]=CTL_NET;
    mib[1]=AF_ROUTE;
    mib[2]=0;
    mib[3]=family; /* only addresses of this family */
    mib[4]=type;
    mib[5]=flags; /* not looked at with NET_RT_DUMP */
    if (sysctl(mib, 6, NULL, lenp, NULL, 0) < 0)
        return (NULL);

    if ((buf=malloc(*lenp)) == NULL)
        return (NULL);
    if (sysctl(mib, 6, buf, lenp, NULL, 0) < 0)
        return (NULL);

    return (buf);
}

/* Performs a route table dump selecting only entries that have Gateways.
 This means that the return buffer will have many duplicate entries since
 certain gateways appear multiple times in the routing table.

 It is up to the caller to weed out duplicates
 */
int getgateways(struct sockaddr_in6 **gws)
{
    char *buf, *next, *lim;
    size_t len;
    struct rt_msghdr *rtm;
    struct sockaddr *sa, *rti_info[RTAX_MAX];
    int rtcount=0;

    if (!gws) {
        return PCP_ERR_UNKNOWN;
    }

    /* net_rt_dump() will return all route entries with gateways */
    buf=net_rt_dump(NET_RT_FLAGS, 0, RTF_GATEWAY, &len);
    if (!buf)
        return PCP_ERR_UNKNOWN;
    lim=buf + len;
    for (next=buf; next < lim; next+=rtm->rtm_msglen) {
        rtm=(struct rt_msghdr *)next;
        sa=(struct sockaddr *)(rtm + 1);
        get_rtaddrs(rtm->rtm_addrs, sa, rti_info);

        if ((sa=rti_info[RTAX_GATEWAY]) != NULL)

            if ((rtm->rtm_addrs & (RTA_DST | RTA_GATEWAY))
                    == (RTA_DST | RTA_GATEWAY)) {
                struct sockaddr_in6 *in6=*gws;

                *gws=(struct sockaddr_in6 *)realloc(*gws,
                        sizeof(struct sockaddr_in6) * (rtcount + 1));

                if (!*gws) {
                    if (in6)
                        free(in6);
                    free(buf);
                    return PCP_ERR_NO_MEM;
                }

                in6=(*gws) + rtcount;
                memset(in6, 0, sizeof(struct sockaddr_in6));

                if (sa->sa_family == AF_INET) {
                    /* IPv4 gateways as returned as IPv4 mapped IPv6 addresses */
                    in6->sin6_family = AF_INET6;
                    S6_ADDR32(&in6->sin6_addr)[0]=
                            ((struct sockaddr_in *)(rti_info[RTAX_GATEWAY]))->sin_addr.s_addr;
                    TO_IPV6MAPPED(&in6->sin6_addr);
                } else if (sa->sa_family == AF_INET6) {
                    memcpy(in6,
                            (struct sockaddr_in6 *)rti_info[RTAX_GATEWAY],
                            sizeof(struct sockaddr_in6));
                } else {
                    continue;
                }
                rtcount++;
            }
    }
    free(buf);
    return rtcount;
}

#if 0

/* This structure is used for route operations using routing sockets */

/* I know I could have used sockaddr. But type casting eveywhere is nasty and error prone.
   I do not believe a few bytes will make much impact and code will become much cleaner */

typedef struct route_op {
    struct sockaddr_in dst4;
    struct sockaddr_in mask4;
    struct sockaddr_in gw4;
    struct sockaddr_in6 dst6;
    struct sockaddr_in6 mask6;
    struct sockaddr_in6 gw6;
    char ifname[128];
    uint16_t family;
} route_op_t;

static int
get_if_addr_from_name(char *ifname, struct sockaddr *ifsock, int family);

typedef route_op_t route_in_t;
typedef route_op_t route_out_t;

static int route_get(in_addr_t *dst, in_addr_t *mask, in_addr_t *gateway,
        char *ifname, route_in_t *routein, route_out_t *routeout);
static int route_add(in_addr_t dst, in_addr_t mask, in_addr_t gateway,
        const char *ifname, route_in_t *routein, route_out_t *routeout);
static int route_change(in_addr_t dst, in_addr_t mask, in_addr_t gateway,
        const char *ifname, route_in_t *routein, route_out_t *routeout);
static int route_delete(in_addr_t dst, in_addr_t mask, route_in_t *routein,
        route_out_t *routeout);
static int route_get_sa(struct sockaddr *dst, in_addr_t *mask,
        struct sockaddr *gateway, char *ifname, route_in_t *routein,
        route_out_t *routeout);

static int find_if_with_name(const char *iface, struct sockaddr_dl *out)
{
    struct ifaddrs *ifap, *ifa;
    struct sockaddr_dl *sdl=NULL;

    if (getifaddrs(&ifap)) {
        char err_msg[128];
        pcp_strerror(errno, err_msg, sizeof(err_msg));
        PCP_LOG(PCP_LOGLVL_DEBUG, "getifaddrs: %s", err_msg);
        return -1;
    }

    for (ifa=ifap; ifa; ifa=ifa->ifa_next) {
        if (ifa->ifa_addr->sa_family == AF_LINK &&
        /*(ifa->ifa_flags & IFF_POINTOPOINT) && \ */
        strcmp(iface, ifa->ifa_name) == 0) {
            sdl=(struct sockaddr_dl *)ifa->ifa_addr;
            break;
        }
    }

    /* If we found it, then use it */
    if (sdl)
        bcopy((char *)sdl, (char *)out, (size_t)(sdl->sdl_len));

    freeifaddrs(ifap);

    if (sdl == NULL) {
        PCP_LOG(PCP_LOGLVL_DEBUG,
                "interface %s not found or invalid(must be p-p)", iface);
        return -1;
    }
    return 0;
}

static int route_op(u_char op, in_addr_t *dst, in_addr_t *mask,
        in_addr_t *gateway, char *iface, route_in_t *routein,
        route_out_t *routeout)
{

#define ROUNDUP_CT(n)  ((n) > 0 ? (1 + (((n) - 1) | (sizeof(uint32_t) - 1))) : sizeof(uint32_t))
#define ADVANCE_CT(x, n) (x += ROUNDUP_CT((n)->sa_len))

#define NEXTADDR_CT(w, u) \
    if (msg.msghdr.rtm_addrs & (w)) {\
        len = ROUNDUP_CT(u.sa.sa_len); bcopy((char *)&(u), cp, len); cp += len;\
    }

    static int seq=0;
    int err=0;
    ssize_t len=0;
    char *cp;
    pid_t pid;

    union {
        struct sockaddr sa;
        struct sockaddr_in sin;
        struct sockaddr_dl sdl;
        struct sockaddr_storage ss; /* added to avoid memory overrun */
    } so_addr[RTAX_MAX];

    struct {
        struct rt_msghdr msghdr;
        char buf[512];
    } msg;

    bzero(so_addr, sizeof(so_addr));
    bzero(&msg, sizeof(msg));

    PCP_LOG_BEGIN(PCP_LOGLVL_DEBUG);
    cp=msg.buf;
    pid=getpid();
    //msg.msghdr.rtm_msglen  = 0;
    msg.msghdr.rtm_version=RTM_VERSION;
    //msg.msghdr.rtm_type    = RTM_ADD;
    msg.msghdr.rtm_index=0;
    msg.msghdr.rtm_pid=pid;
    msg.msghdr.rtm_addrs=0;
    msg.msghdr.rtm_seq=++seq;
    msg.msghdr.rtm_errno=0;
    msg.msghdr.rtm_flags=0;

    // Destination
    /* if (dst && *dst != 0xffffffff) { */
    if (routein && routein->dst4.sin_addr.s_addr != 0xffffffff) {
        msg.msghdr.rtm_addrs|=RTA_DST;

        so_addr[RTAX_DST].sin.sin_len=sizeof(struct sockaddr_in);
        so_addr[RTAX_DST].sin.sin_family=AF_INET;
        so_addr[RTAX_DST].sin.sin_addr.s_addr=mask ? *dst & *mask : *dst;
    } else {
        PCP_LOG(PCP_LOGLVL_DEBUG, "%s", "invalid(require) dst address");
        return -1;
    }

    // Netmask
    if (mask && *mask != 0xffffffff) {
        msg.msghdr.rtm_addrs|=RTA_NETMASK;

        so_addr[RTAX_NETMASK].sin.sin_len=sizeof(struct sockaddr_in);
        so_addr[RTAX_NETMASK].sin.sin_family=AF_INET;
        so_addr[RTAX_NETMASK].sin.sin_addr.s_addr=*mask;

    } else
        msg.msghdr.rtm_flags|=RTF_HOST;

    switch (op) {
        case RTM_ADD:
        case RTM_CHANGE:
            msg.msghdr.rtm_type=op;
            msg.msghdr.rtm_addrs|=RTA_GATEWAY;
            msg.msghdr.rtm_flags|=RTF_UP;

            // Gateway
            if ((gateway && *gateway != 0x0 && *gateway != 0xffffffff)) {
                msg.msghdr.rtm_flags|=RTF_GATEWAY;

                so_addr[RTAX_GATEWAY].sin.sin_len=sizeof(struct sockaddr_in);
                so_addr[RTAX_GATEWAY].sin.sin_family=AF_INET;
                so_addr[RTAX_GATEWAY].sin.sin_addr.s_addr=*gateway;

                if (iface != NULL) {
                    msg.msghdr.rtm_addrs|=RTA_IFP;
                    so_addr[RTAX_IFP].sdl.sdl_family=AF_LINK;

                    //link_addr(iface, &so_addr[RTAX_IFP].sdl);
                    if (find_if_with_name(iface, &so_addr[RTAX_IFP].sdl) < 0)
                        return -2;
                }

            } else {
                if (iface == NULL) {
                    PCP_LOG(PCP_LOGLVL_DEBUG, "%s",
                            "Requir gateway or iface.");
                    return -1;
                }

                if (find_if_with_name(iface, &so_addr[RTAX_GATEWAY].sdl) < 0)
                    return -1;
            }
            break;
        case RTM_DELETE:
            msg.msghdr.rtm_type=op;
            msg.msghdr.rtm_addrs|=RTA_GATEWAY;
            msg.msghdr.rtm_flags|=RTF_GATEWAY;
            break;
        case RTM_GET:
            msg.msghdr.rtm_type=op;
            msg.msghdr.rtm_addrs|=RTA_IFP;
            so_addr[RTAX_IFP].sa.sa_family=AF_LINK;
            so_addr[RTAX_IFP].sa.sa_len=sizeof(struct sockaddr_dl);
            break;
        default:
            return EINVAL;
    }

    NEXTADDR_CT(RTA_DST, so_addr[RTAX_DST]);
    NEXTADDR_CT(RTA_GATEWAY, so_addr[RTAX_GATEWAY]);
    NEXTADDR_CT(RTA_NETMASK, so_addr[RTAX_NETMASK]);
    NEXTADDR_CT(RTA_GENMASK, so_addr[RTAX_GENMASK]);
    NEXTADDR_CT(RTA_IFP, so_addr[RTAX_IFP]);
    NEXTADDR_CT(RTA_IFA, so_addr[RTAX_IFA]);

    msg.msghdr.rtm_msglen=len=cp - (char *)&msg;

    int sock=socket(PF_ROUTE, SOCK_RAW, AF_INET);
    if (sock == -1) {
        PCP_LOG(PCP_LOGLVL_DEBUG, "%s",
                "socket(PF_ROUTE, SOCK_RAW, AF_INET) failed");
        return -1;
    }

    if (write(sock, (char *)&msg, len) < 0) {
        err=-1;
        goto end;
    }

    if (op == RTM_GET) {
        do {
            len=read(sock, (char *)&msg, sizeof(msg));
        } while (len > 0
                && (msg.msghdr.rtm_seq != seq || msg.msghdr.rtm_pid != pid));

        if (len < 0) {
            PCP_LOG(PCP_LOGLVL_DEBUG, "%s",
                    "read from routing socket failed");
            err=-1;
        } else {
            struct sockaddr *s_netmask=NULL;
            struct sockaddr *s_gate=NULL;
            struct sockaddr_dl *s_ifp=NULL;
            struct sockaddr *sa;
            struct sockaddr *rti_info[RTAX_MAX];

            if (msg.msghdr.rtm_version != RTM_VERSION) {
                PCP_LOG(PCP_LOGLVL_DEBUG,
                        "routing message version %d not understood",
                        msg.msghdr.rtm_version);
                err=-1;
                goto end;
            }
            if (msg.msghdr.rtm_msglen > len) {
                PCP_LOG(PCP_LOGLVL_DEBUG,
                        "message length mismatch, in packet %d, returned %lu",
                        msg.msghdr.rtm_msglen, (unsigned long)len);
            }
            if (msg.msghdr.rtm_errno) {
                PCP_LOG(PCP_LOGLVL_DEBUG, "message indicates error %d, %s",
                        msg.msghdr.rtm_errno, strerror(msg.msghdr.rtm_errno));
                err=-1;
                goto end;
            }
            cp=msg.buf;
            if (msg.msghdr.rtm_addrs) {

                sa=(struct sockaddr *)cp;
                get_rtaddrs(msg.msghdr.rtm_addrs, sa, rti_info);

                if ((sa=rti_info[RTAX_DST]) != NULL) {
                    if (sa->sa_family == AF_INET) {
                        char routeto4_str[INET_ADDRSTRLEN];

                        memcpy((void *)&(routeout->dst4), (void *)sa,
                                sizeof(struct sockaddr_in));
                        inet_ntop(AF_INET, &(routein->dst4.sin_addr),
                                routeto4_str, INET_ADDRSTRLEN);
                        //memcpy(routeto4_str, sock_ntop((struct sockaddr *)&(routein->dst4), sizeof(struct sockaddr_in)),
                        //        INET_ADDRSTRLEN);
                        /* BSD uses a peculiar nomenclature. 'Route to' is the host for which we are looking up
                         the route and 'destinaton' is the intermediate gateway or the final destination
                         when hosts are in the same LAN */
                        PCP_LOG(PCP_LOGLVL_DEBUG, "Dest: %s",
                                sock_ntop(sa, sa->sa_len));
                        if (msg.msghdr.rtm_flags & RTF_WASCLONED) {
                            PCP_LOG(PCP_LOGLVL_DEBUG,
                                    "Route to %s in the same subnet",
                                    routeto4_str);
                        } else if (msg.msghdr.rtm_flags & RTF_CLONING) {
                            PCP_LOG(PCP_LOGLVL_DEBUG,
                                    "Route to %s in the same subnet but not up",
                                    routeto4_str);
                        } else if (msg.msghdr.rtm_flags & RTF_GATEWAY) {
                            PCP_LOG(PCP_LOGLVL_DEBUG,
                                    "Route to %s not in the same subnet",
                                    routeto4_str);
                        }
                        if (routeout->dst4.sin_addr.s_addr
                                == routein->dst4.sin_addr.s_addr) {
                            PCP_LOG(PCP_LOGLVL_DEBUG,
                                    "Destination %s same as route to %s",
                                    sock_ntop(sa, sa->sa_len), routeto4_str);
                        }
                    }
                }

                if ((sa=rti_info[RTAX_GATEWAY]) != NULL) {
                    s_gate=sa;
                    PCP_LOG(PCP_LOGLVL_DEBUG, "Gateway: %s \n",
                            sock_ntop(sa, sa->sa_len));
                    if (msg.msghdr.rtm_flags & RTF_GATEWAY) {
                        *gateway=
                                ((struct sockaddr_in *)s_gate)->sin_addr.s_addr;
                        routeout->gw4.sin_addr.s_addr=
                                ((struct sockaddr_in *)s_gate)->sin_addr.s_addr;
                    } else {
                        *gateway=0;
                    }

                }

                if ((sa=rti_info[RTAX_IFP]) != NULL) {
                    PCP_LOG(PCP_LOGLVL_DEBUG,
                            "family: %d AF_LINK family : %d \n",
                            sa->sa_family, AF_LINK);
                    if ((sa->sa_family == AF_LINK)
                            && ((struct sockaddr_dl *)sa)->sdl_nlen) {
                        uint32_t slen;
                        s_ifp=(struct sockaddr_dl *)sa;
                        slen=s_ifp->sdl_nlen < IFNAMSIZ ?
                                s_ifp->sdl_nlen : IFNAMSIZ;
                        strncpy(iface, s_ifp->sdl_data, slen);
                        strncpy(&(routeout->ifname[0]), s_ifp->sdl_data, slen);
                        routeout->ifname[slen]='\0';
                        iface[slen]='\0';
                        PCP_LOG(PCP_LOGLVL_DEBUG,
                                "Interface name %s and type %d \n",
                                &(routeout->ifname[0]), s_ifp->sdl_type);
                    }
                }
            }

            if (mask) {
                if (*dst == 0)
                    *mask=0;
                else if (s_netmask)
                    *mask=((struct sockaddr_in *)s_netmask)->sin_addr.s_addr; // there must be something wrong here....Ah..
                else
                    *mask=0xffffffff; // it is a host
            }

#if 0
            if (iface && s_ifp) {
                strncpy(iface, s_ifp->sdl_data,
                        s_ifp->sdl_nlen < IFNAMSIZ ?
                                s_ifp->sdl_nlen : IFNAMSIZ);
                iface[IFNAMSIZ - 1]='\0';
            }
#endif
        }
    }

end:
    if (close(sock) < 0) {
        PCP_LOG(PCP_LOGLVL_DEBUG, "%s", "socket close failed");
    }

    PCP_LOG_END(PCP_LOGLVL_DEBUG);
    return err;
#undef MAX_INDEX
}

static int route_get_sa(struct sockaddr *dst, in_addr_t *mask,
        struct sockaddr *gateway, char *ifname, route_in_t *routein,
        route_out_t *routeout)
{
#if defined(__APPLE__) || defined(__FreeBSD__)
    return route_op(RTM_GET, &(((struct sockaddr_in *) dst)->sin_addr.s_addr), mask,
            &(((struct sockaddr_in *) gateway)->sin_addr.s_addr), ifname, routein, routeout);
#else
    PCP_LOG(PCP_LOGLVL_DEBUG, "%s: todo...\n", __FUNCTION__);
    return 0;
#endif
}

static int route_get(in_addr_t *dst, in_addr_t *mask, in_addr_t *gateway,
        char iface[], route_in_t *routein, route_out_t *routeout)
{
#if defined(__APPLE__) || defined(__FreeBSD__)
    return route_op(RTM_GET, dst, mask, gateway, iface, routein, routeout);
#else
    PCP_LOG(PCP_LOGLVL_DEBUG, "%s: todo...\n", __FUNCTION__);
    return 0;
#endif
}

static int route_add(in_addr_t dst, in_addr_t mask, in_addr_t gateway,
        const char *iface, route_in_t *routein, route_out_t *routeout)
{
#if defined(__APPLE__) || defined(__FreeBSD__)
    return route_op(RTM_ADD, &dst, &mask, &gateway, (char *)iface, routein, routeout);
#else
    PCP_LOG(PCP_LOGLVL_DEBUG, "%s: todo...\n", __FUNCTION__);
    return -1;
#endif
}

static int route_change(in_addr_t dst, in_addr_t mask, in_addr_t gateway,
        const char *iface, route_in_t *routein, route_out_t *routeout)
{
#if defined(__APPLE__) || defined(__FreeBSD__)
    return route_op(RTM_CHANGE, &dst, &mask, &gateway, (char *)iface, routein, routeout);
#else
    PCP_LOG(PCP_LOGLVL_DEBUG, "%s: todo...\n", __FUNCTION__);
    return -1;
#endif
}

static int route_delete(in_addr_t dst, in_addr_t mask, route_in_t *routein,
        route_out_t *routeout)
{
#if defined(__APPLE__) || defined(__FreeBSD__)
    return route_op(RTM_DELETE, &dst, &mask, 0, NULL, routein, routeout);
#else
    PCP_LOG(PCP_LOGLVL_DEBUG, "%s: todo...\n", __FUNCTION__);
    return -1;
#endif
}

/* Adapted from Linux manual example */

/* We need to pass the family because an interface might have multiple addresses
 each assocaited with different family
 */
static int get_if_addr_from_name(char *ifname, struct sockaddr *ifsock,
        int family)
{
    struct ifaddrs *ifaddr, *ifa;

    if (getifaddrs(&ifaddr) == -1) {
        char err_msg[128];
        pcp_strerror(errno, err_msg, sizeof(err_msg));
        PCP_LOG(PCP_LOGLVL_DEBUG, "getifaddrs: %s", err_msg);
        return -1;
    }

    /* Walk through linked list, maintaining head pointer so we
     can free list later */

    for (ifa=ifaddr; ifa != NULL; ifa=ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;

        /* For an AF_INET* interface address, display the address */
        if (!strcmp(ifname, ifa->ifa_name)
                && (ifa->ifa_addr->sa_family == family)) {
            memcpy(ifsock, ifa->ifa_addr, sizeof(struct sockaddr));
            freeifaddrs(ifaddr);
            return 0;
        }

    }

    freeifaddrs(ifaddr);
    return -1;
}
#endif //0

#endif
