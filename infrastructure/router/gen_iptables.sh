#!/bin/bash

N=${1?no teams count}
inet_ifaces=${2? no inet ifaces}
team2team=$([ -n "$3" ] && echo true || echo false)

vpn=172.16.19/24

core_switch=10.24/24
team_switches=10.24/19

checksystem=10.23/24
devs=10.23.$N/24
teams=10.23/19

any=0/0

iptables -P FORWARD ACCEPT

init_chain() {
    local table=$1 chain=$2
    local ipt="iptables -t $table"
    $ipt -L $chain &>/dev/null && $ipt -F $chain || $ipt -N $chain
}

init_chain_pair() {
    local table=$1 chain=$2
    init_chain $table from_$chain
    init_chain $table to_$chain
}

init_chain filter ructf2014
init_chain_pair filter inet
for iface in $inet_ifaces; do
    iptables -t filter -A to_inet -o $iface -j ACCEPT
done

init_network() {
    local name=${1%%:*} network=${1##*:}
    init_chain_pair filter $name
    iptables -t filter -A ructf2014 -s $network -j from_$name
    iptables -t filter -A to_$name -d $network -j ACCEPT
}

# the order of networks here is important
# as it goes to ructf2014 chain as from_* targets in that order
for net in vpn core_switch devs checksystem teams team_switches any; do
    init_network $net:${!net}
done

# and we fix networks here additionally in to_* chains
# exclude core switch network from team switches
iptables -t filter -I to_team_switches 1 -d $core_switch -j RETURN

# exclude devs and checksystem networks from teams
iptables -t filter -I to_teams 1 -d $devs -j RETURN
iptables -t filter -I to_teams 1 -d $checksystem -j RETURN

# and here we go, THE RULES
can_go() { iptables -t filter -A from_$1 -j to_$2; }

can_go vpn any

can_go devs inet
can_go devs teams
can_go devs checksystem

can_go checksystem inet
can_go checksystem teams

can_go teams inet
$team2team && can_go teams teams

#iptables -P FORWARD DROP
