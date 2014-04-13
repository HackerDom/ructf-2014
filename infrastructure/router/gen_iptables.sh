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

add_filter='iptables -t filter -A'
add_nat='iptables -t nat -A'

# first, INPUT
iptables -P INPUT ACCEPT
iptables -F INPUT

$add_filter INPUT -m state --state RELATED,ESTABLISHED -j ACCEPT
$add_filter INPUT -p icmp -j ACCEPT
$add_filter INPUT -i lo -j ACCEPT

$add_filter INPUT -s $vpn -j ACCEPT
for iface in $inet_ifaces; do
    # only allow SSH on external ifaces
    $add_filter INPUT -i $iface -m state --state NEW -p tcp --dport 22 -j ACCEPT
done
iptables -P INPUT DROP


# Forwarding
iptables -P FORWARD ACCEPT
iptables -F FORWARD

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

init_chain filter accept_new
$add_filter accept_new -p tcp -m state --state NEW -j ACCEPT
$add_filter accept_new -p udp -m state --state NEW -j ACCEPT

init_chain_pair filter inet
for iface in $inet_ifaces; do
    $add_filter to_inet -o $iface -j accept_new
done

init_network() {
    local name=${1%%:*} network=${1##*:}
    init_chain_pair filter $name
    $add_filter ructf2014 -s $network -j from_$name
    $add_filter to_$name -d $network -j accept_new
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
can_go() { $add_filter from_$1 -j to_$2; }

can_go vpn any

can_go devs inet
can_go devs teams
can_go devs checksystem

can_go checksystem inet
can_go checksystem teams

can_go teams inet
can_go teams devs
$team2team && can_go teams teams

$add_filter FORWARD -m state --state RELATED,ESTABLISHED -j ACCEPT
$add_filter FORWARD -p icmp -j ACCEPT
$add_filter FORWARD -j ructf2014

iptables -P FORWARD DROP


# NAT
init_chain nat ructf2014

init_chain nat to_teams
for dst in vpn devs checksystem; do
    iptables -t nat -A to_teams -d ${!dst} -j ACCEPT
done
$add_nat to_teams -d $teams -j MASQUERADE
$add_nat ructf2014 -j to_teams

init_chain nat to_inet
for iface in $inet_ifaces; do
    $add_nat to_inet -o $iface -j MASQUERADE
done
$add_nat ructf2014 -j to_inet

iptables -t nat -F POSTROUTING
$add_nat POSTROUTING -j ructf2014
