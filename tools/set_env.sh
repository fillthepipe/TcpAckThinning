#!/bin/bash

[[ $UID -ne 0 ]] && echo "Need to be root to do this" && exit 1
sysctl -w net.ipv4.tcp_congestion_control=bbr
#sysctl -w net.ipv4.tcp_congestion_control=cubic
ethtool --offload wlp60s0 gso off tso off gro off
adb shell /data/ethtool --offload wlan0 gso off tso off gro off
