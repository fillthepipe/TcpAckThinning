#!/bin/bash

set -E
trap 'echo Failed line: $LINENO command: $BASH_COMMAND && exit $?' ERR

lcnt=1
nw_pref="192.168.10"
create_node()
{
    ns="$1"; shift;

    ip netns add $ns
    ip netns exec $ns ip link set lo up

    while [ "$1" != "" ]; do
        hostlink="hostlink$lcnt";
        ip link add $hostlink type veth peer name $1
        ip link set $1 netns $ns
        ip addr add $nw_pref${lcnt}.1/24 dev $hostlink
        ip link set $hostlink up
        ip netns exec $ns ip addr add $nw_pref${lcnt}.2/24 dev $1
        ip netns exec $ns ip link set $1 up

        ip netns exec $ns echo "2$lcnt out$lcnt" >> /etc/iproute2/rt_tables
        ip netns exec $ns ip route add default via $nw_pref${lcnt}.1 table out$lcnt
        ip netns exec $ns ip rule add from $nw_pref${lcnt}.2 table out$lcnt
        ip netns exec $ns route add default gateway $nw_pref${lcnt}.1

        ((lcnt++)); shift;
    done
}

main()
{
    ip -all netns delete
    [[ "$(ip netns)" != "" ]] && echo "could not delete all existing namespaces..." && exit 1
    sysctl net.ipv4.ip_forward=1 >/dev/null
    create_node "cli" "wifi"
#    create_node "tgw"    "eth0" "eth1"
#    create_node "srvpxy" "eth0"
    create_node "srv" "eth0"
}

main
