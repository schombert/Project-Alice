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

#ifndef PCP_UTILS_H_
#define PCP_UTILS_H_

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "pcp_logger.h"
#include "pcp_client_db.h"

 #ifdef __GNUC__
  #define UNUSED __attribute__ ((unused))
 #else
  #define UNUSED
 #endif

#ifdef _MSC_VER
/* variable num of arguments*/
#define DUPPRINT(fp, fmt, ...)    \
    do {                        \
        printf(fmt, __VA_ARGS__);            \
        if (fp != NULL) {       \
            fprintf(fp, fmt, __VA_ARGS__);    \
        }                       \
    } while(0)
#else /*WIN32*/
#define DUPPRINT(fp, fmt...)    \
    do {                        \
        printf(fmt);            \
        if (fp != NULL) {       \
            fprintf(fp,fmt);    \
        }                       \
    } while(0)
#endif /*WIN32*/

#define log_err(STR) \
    do {        \
        printf("%s:%d "#STR": %s \n", __FUNCTION__, __LINE__, strerror(errno)); \
    } while (0)

#define log_debug_scr(STR) \
    do {        \
        printf("%s:%d %s \n", __FUNCTION__, __LINE__, STR); \
    } while (0)

#define log_debug(STR) \
    do {        \
        printf("%s:%d "#STR" \n", __FUNCTION__, __LINE__); \
    } while (0)

#define CHECK_RET_EXIT(func)        \
    do {                            \
        if (func < 0) {             \
            log_err("");            \
            exit (EXIT_FAILURE);    \
        }                           \
    } while(0)

#define CHECK_NULL_EXIT(func)       \
    do {                            \
        if (func == NULL) {         \
            log_err("");            \
            exit (EXIT_FAILURE);    \
        }                           \
    } while(0)


#define CHECK_RET(func)             \
    do {                            \
        if (func < 0) {             \
            log_err("");            \
        }                           \
    } while(0)

#define CHECK_RET_GOTO_ERROR(func)  \
    do {                            \
        if (func < 0) {             \
            log_err("");            \
            goto ERROR;             \
        }                           \
    } while(0)



#define OSDEP(x) (void)(x)

#ifdef s6_addr32
#define S6_ADDR32(sa6) (sa6)->s6_addr32
#else
#define S6_ADDR32(sa6) ((uint32_t *)((sa6)->s6_addr))
#endif

#define IPV6_ADDR_COPY(dest, src)   \
    do {                            \
        (S6_ADDR32(dest))[0]=(S6_ADDR32(src))[0];       \
        (S6_ADDR32(dest))[1]=(S6_ADDR32(src))[1];       \
        (S6_ADDR32(dest))[2]=(S6_ADDR32(src))[2];       \
        (S6_ADDR32(dest))[3]=(S6_ADDR32(src))[3];       \
    } while (0)

#include "pcp_msg.h"
static inline int compare_epochs(pcp_recv_msg_t *f, pcp_server_t *s)
{
    uint32_t c_delta;
    uint32_t s_delta;

    if (s->epoch == ~0u) {
        s->epoch=f->recv_epoch;
        s->cepoch=f->received_time;
    }
    c_delta=(uint32_t)(f->received_time - s->cepoch);
    s_delta=f->recv_epoch - s->epoch;

    PCP_LOG(PCP_LOGLVL_DEBUG,
            "Epoch - client delta = %u, server delta = %u",
            c_delta, s_delta);

    return (c_delta + 2 < s_delta - (s_delta >> 4))
            || (s_delta + 2 < c_delta - (c_delta >> 4));
}

inline static void timeval_align(struct timeval *x)
{
    x->tv_sec+=x->tv_usec / 1000000;
    x->tv_usec=x->tv_usec % 1000000;
    if (x->tv_usec<0) {
        x->tv_usec=1000000 + x->tv_usec;
        x->tv_sec-=1;
    }
}

inline static int timeval_comp(struct timeval *x, struct timeval *y)
{
    timeval_align(x);
    timeval_align(y);
    if (x->tv_sec < y->tv_sec) {
        return -1;
    } else if (x->tv_sec > y->tv_sec) {
        return 1;
    } else if (x->tv_usec < y->tv_usec) {
        return -1;
    } else if (x->tv_usec > y->tv_usec) {
        return 1;
    } else {
        return 0;
    }
}

inline static int timeval_subtract(struct timeval *result, struct timeval *x,
        struct timeval *y)
{
    int ret=timeval_comp(x, y);

    if (ret<=0) {
        result->tv_sec=0;
        result->tv_usec=0;
        return 1;
    }

    // in case that tv_usec is unsigned -> perform the carry
    if (x->tv_usec < y->tv_usec) {
        int nsec=(y->tv_usec - x->tv_usec) / 1000000 + 1;
        y->tv_usec-=1000000 * nsec;
        y->tv_sec+=nsec;
    }

    /* Compute the time remaining to wait.
       tv_usec is certainly positive. */
    result->tv_sec=x->tv_sec - y->tv_sec;
    result->tv_usec=x->tv_usec - y->tv_usec;
    timeval_align(result);

    /* Return 1 if result is negative. */
    return ret <= 0;
}

/* Nonce is part of the MAP and PEER requests/responses
   as of version 2 of the PCP protocol */
static inline void createNonce(struct pcp_nonce *nonce_field)
{
    int i;
    for (i = 2; i >= 0; --i)
#ifdef WIN32
        nonce_field->n[i]=htonl (rand());
#else  //WIN32
        nonce_field->n[i]=htonl(random());
#endif //WIN32
}

#ifndef HAVE_STRNDUP
static inline char *pcp_strndup(const char *s, size_t size) {
  char *ret;
  char const* end=(char const*)memchr(s, 0, size);

  if (end) {
    /* Length + 1 */
    size=end - s + 1;
  } else {
    size++;
  }
  ret=(char*)malloc(size);

  if (ret) {
      memcpy(ret, s, size);
      ret[size-1]='\0';
  }
  return ret;
}
#define strndup pcp_strndup
#endif

#endif /* PCP_UTILS_H_ */
