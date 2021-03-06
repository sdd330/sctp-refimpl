/*-
 * Copyright (C) 2002, 2004 Cisco Systems Inc,
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/* $KAME: sctp_timer.h,v 1.6 2005/03/06 16:04:18 itojun Exp $	 */
#ifdef __FreeBSD__
#include <sys/cdefs.h>
__FBSDID("$FreeBSD:$");
#endif

#ifndef __sctp_timer_h__
#define __sctp_timer_h__


#if (defined(__APPLE__) && defined(KERNEL))
#ifndef _KERNEL
#define _KERNEL
#endif
#endif

#if defined(_KERNEL)

void
sctp_early_fr_timer(struct sctp_inpcb *inp, struct sctp_tcb *stcb,
    struct sctp_nets *net);

struct sctp_nets *
sctp_find_alternate_net(struct sctp_tcb *,
    struct sctp_nets *, int high_ssthresh);

int
sctp_threshold_management(struct sctp_inpcb *, struct sctp_tcb *,
    struct sctp_nets *, uint16_t);

int
sctp_t3rxt_timer(struct sctp_inpcb *, struct sctp_tcb *,
    struct sctp_nets *);
int
sctp_t1init_timer(struct sctp_inpcb *, struct sctp_tcb *,
    struct sctp_nets *);
int
sctp_shutdown_timer(struct sctp_inpcb *, struct sctp_tcb *,
    struct sctp_nets *);
int
sctp_heartbeat_timer(struct sctp_inpcb *, struct sctp_tcb *,
    struct sctp_nets *, int);

int sctp_is_hb_timer_running(struct sctp_tcb *stcb);
int sctp_is_sack_timer_running(struct sctp_tcb *stcb);

int
sctp_cookie_timer(struct sctp_inpcb *, struct sctp_tcb *,
    struct sctp_nets *);

void
sctp_pathmtu_timer(struct sctp_inpcb *, struct sctp_tcb *,
    struct sctp_nets *);

int
sctp_shutdownack_timer(struct sctp_inpcb *, struct sctp_tcb *,
    struct sctp_nets *);
int
sctp_strreset_timer(struct sctp_inpcb *inp, struct sctp_tcb *stcb,
    struct sctp_nets *net);

int
sctp_asconf_timer(struct sctp_inpcb *, struct sctp_tcb *,
    struct sctp_nets *);

void
sctp_autoclose_timer(struct sctp_inpcb *, struct sctp_tcb *,
    struct sctp_nets *net);

void sctp_audit_retranmission_queue(struct sctp_association *);

void sctp_iterator_timer(struct sctp_iterator *it);

#ifdef SCTP_APPLE_FINE_GRAINED_LOCKING
void sctp_slowtimo();

#endif

#endif
#endif
