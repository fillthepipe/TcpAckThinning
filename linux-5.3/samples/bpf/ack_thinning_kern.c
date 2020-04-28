/* Copyright (c) 2013-2015 PLUMgrid, http://plumgrid.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of version 2 of the GNU General Public
 * License as published by the Free Software Foundation.
 */

#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <uapi/linux/bpf.h>
#include <linux/version.h>
#include "bpf_helpers.h"
#include "bpf_endian.h"

SEC("sockops")
int bpf_bufs(struct bpf_sock_ops *skops)
{
    char fmt[] = "port: rem=%d loc=%d\n";
    int quickack = 1;

    skops->reply = -1;
    if (skops->op != BPF_SOCK_OPS_ACK_THRESH_INIT) {
		return 1;
    }

    bpf_trace_printk(fmt, sizeof(fmt), bpf_ntohl(skops->remote_port),
            skops->local_port);

	if (bpf_ntohl(skops->remote_port) != 12345 &&
	    skops->local_port != 12345) {
		return 1;
	}

    skops->reply = 1*1448; // L * MSS
	bpf_printk("setting threshold %d\n", skops->reply);
	return 1;
}

char _license[] SEC("license") = "GPL";
u32 _version SEC("version") = LINUX_VERSION_CODE;
