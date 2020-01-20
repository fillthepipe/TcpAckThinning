#!/bin/bash
./pcap_stats.sh cap &
sudo bpftrace ack_check.bt

