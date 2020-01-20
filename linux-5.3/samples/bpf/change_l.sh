#!/bin/bash

sed -i "s/=.*1448/= $1*1448/g" ack_thinning_kern.c
make
for((i=${1}190,j=0;j<10;i++,j++)); do
	name=ack$i;
	bpftool prog load ack_thinning_kern.o /sys/fs/bpf/$name
	bpftool cgroup attach /tmp/cgroupv2/ack sock_ops pinned /sys/fs/bpf/$name
	curl --output /dev/null  http://192.168.43.1:12345/Download/500MB
	kill -2 `pgrep bpftrace`
	sleep 5
done
