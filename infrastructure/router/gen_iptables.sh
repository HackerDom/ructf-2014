#!/bin/bash

N=${1?no teams count}

iptables -P FORWARD ACCEPT

init_chain() {
    local table=$1 chain=$2
    ipt="iptables -t $table"
    $ipt -L $chain &>/dev/null && \
        $ipt -F $chain || \
        $ipt -N $chain
}

init_chain_pair() {
    local chain=$1
    init_chain filter from_$chain
    init_chain filter to_$chain
}

init_network() {
    local name=${1%%:*} network=${1##*:}
    init_chain_pair $name
    iptables -A from_$name -s $network -j ACCEPT
    iptables -A   to_$name -d $network -j ACCEPT
}

remove_chain() {
    local table=$1 chain=$2
    iptables -t $table -L $chain &>/dev/null && {
        iptables -t $table -F $chain
        iptables -t $table -X $chain
    }
}

for net in vpn:172.16.19.0/24 \
           core_switch:10.24.0.0/24 \
           checksystem:10.23.0.0/24; do
    init_network $net
done

for i in $(seq 1 $N); do
    init_network $([ $i -eq $N ] && echo devs || echo team$i):10.23.$i.0/24
    init_network team${i}_sw:10.24.$i.0/24
done

#iptables -P FORWARD DROP
