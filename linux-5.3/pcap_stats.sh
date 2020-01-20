#!/bin/bash

PCAP=tcp.pcap
if [ "$1" == "cap" ]; then
    echo "Started pkt capture..."
    rm $PCAP
    sudo tcpdump -i wlp60s0 -nn -s 80 tcp and port 12345 -w $PCAP 2>/dev/null
else
    [[ ! -f "$1" ]] && echo "Usage: $0 <pcap>" && exit 1
    PCAP="$1"
fi

#tcp_psh=`tcpdump -nn -r $PCAP tcp and src port 12345 2>/dev/null | wc -l`
tcp_ack=`tcpdump -nn -r $PCAP tcp and dst port 12345 and "tcp[13] & 16 != 0" 2>/dev/null | wc -l`

#echo "TCP PUSH rcvd=$tcp_psh"
echo "TCP ACKS sent=$tcp_ack"
