#!/bin/bash
nem=/tmp/ackname
RES=results
tcpdump -i wlp60s0 -nn -s 120 tcp and port 12345 -w tcp.pcap 2>/dev/null &
cap=$!
unbuffer bpftrace ack_check.bt | tee mon.log
kill -2 $cap
if [ -f "$nem" ]; then
    mz=`cat $nem`
    mv tcp.pcap $RES/tcp_$mz.pcap
    mv mon.log $RES/mon_$mz.log
    rm $nem
fi

