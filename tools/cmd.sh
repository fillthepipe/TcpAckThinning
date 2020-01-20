#!/bin/bash

[[ "`whoami`" != "root" ]] && echo "Need to be root (for bpftrace)." && exit 1

FILE=${1-100MB}
IP=${IP-192.168.43.1}
echo "Server $IP"

gcc -shared -fPIC -ldl -o libquickack.so quickack.c
[[ $? -ne 0 ]] && echo "quickack compilation failed" && exit 2
#export LD_PRELOAD=$PWD/libquickack.so

PCAP=tcp_${FILE}.pcap
tcpdump -i wlp60s0 -nn -s 80 tcp and port 12345 -w $PCAP 2>/dev/null &
tpid=$!

bpftrace ack_check.bt &
bpid=$!
sleep 3

wget -O /dev/null http://$IP:12345/Download/$FILE
#curl --output /dev/null http://$IP:12345/Download/$FILE
#curl --output /dev/null http://$IP:12345/Download/base.apk
#curl --output t http://192.168.200.41:12345/Download/base.apk
#curl --local-port 12345 --output t  http://192.168.200.41:12345/Download/base.apk 

kill -2 $bpid $tpid
sleep 2
tcp_ack=`tcpdump -nn -r $PCAP tcp and dst port 12345 and "tcp[13] & 16 != 0" 2>/dev/null | wc -l`
echo "TCP ACKS sent=$tcp_ack"
