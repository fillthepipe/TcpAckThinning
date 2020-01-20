/* Copyright (c) 2013-2015 PLUMgrid, http://plumgrid.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of version 2 of the GNU General Public
 * License as published by the Free Software Foundation.
 */
#define KBUILD_MODNAME "bpftrace"

#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/tcp.h>
#include <net/inet_connection_sock.h>
#include <uapi/linux/bpf.h>
#include <linux/version.h>
#include "bpf_helpers.h"
#include "bpf_endian.h"

/* kprobe is NOT a stable ABI
 * kernel functions can be removed, renamed or completely change semantics.
 * Number of arguments and their positions can change, etc.
 * In such case this bpf+kprobe example will no longer be meaningful
 */
#if 0
SEC("kprobe/__tcp_ack_snd_check")
int bpf_ack_snd_check(struct pt_regs *ctx)
{
    char fmt1[] = "CHECK: diff=%d rcv_nxt:%d rcv_wup:%d\n";
    struct sock *sk;
    struct tcp_sock *tp;
    struct inet_connection_sock *icsk;
    __u16 rcv_mss;
    u32 rcv_nxt; // What we want to receive next?
    u32 rcv_wup; // rcv_nxt on last window update sent
    u32 diff;

	sk = (struct sock *) PT_REGS_PARM1(ctx);
    tp = tcp_sk(sk);
	icsk = inet_csk(sk);
    bpf_probe_read(&rcv_mss, sizeof(rcv_mss), &icsk->icsk_ack.rcv_mss);
    bpf_probe_read(&rcv_nxt, sizeof(rcv_nxt), &tp->rcv_nxt);
    bpf_probe_read(&rcv_wup, sizeof(rcv_wup), &tp->rcv_wup);
    diff = rcv_nxt - rcv_wup;
    bpf_trace_printk(fmt1, sizeof(fmt1), diff, rcv_nxt, rcv_wup);
    return 0;
}
#endif

SEC("kprobe/tcp_send_ack")
int bpf_send_ack(struct pt_regs *ctx)
{
    char fmt[] = "SEND ACK: sk=%p\n";
    struct sock *sk;

	sk = (struct sock *) PT_REGS_PARM1(ctx);
    bpf_trace_printk(fmt, sizeof(fmt), sk);
    return 0;
}

SEC("kretprobe/tcp_initialize_rcv_mss")
int bpf_rcv_mss(struct pt_regs *ctx)
{
    char fmt[] = "INIT RCV_MSS: rcv_mss:%d\n";
    struct sock *sk = (struct sock *)PT_REGS_PARM1(ctx);
    struct tcp_sock *tp;
    struct inet_connection_sock *icsk;
    __u16 rcv_mss;

    tp = tcp_sk(sk);
	icsk = inet_csk(sk);
    bpf_probe_read(&rcv_mss, sizeof(rcv_mss), &icsk->icsk_ack.rcv_mss);
    bpf_trace_printk(fmt, sizeof(fmt), rcv_mss);
    return 0;
}

SEC("kprobe/tcp_connect")
int bpf_ack_snd_check(struct pt_regs *ctx)
{
    char fmt[] = "CONNECT: sk=%p\n";
    struct sock *sk;

	sk = (struct sock *) PT_REGS_PARM1(ctx);
    bpf_trace_printk(fmt, sizeof(fmt), sk);
    return 0;
}

char _license[] SEC("license") = "GPL";
u32 _version SEC("version") = LINUX_VERSION_CODE;
