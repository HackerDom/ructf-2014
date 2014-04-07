#!/bin/bash

DEV=${1?no device}
N=${2?no team number}

SETUP="python $(dirname $0)/setup_hp.py"

DEFAULT_IP=192.168.2.10
DEFAULT_MASK=255.255.255.0
DEFAULT_PREFIX=24
DEFAULT_GW=192.168.2.1

check_ping() {
    local addr=$1 count=$2
    if [ -n "$3" ]; then
        local action=" after $3"
    else
        local action=
    fi
    for ((i=0; i!=count; ++i)); do
        ping -c 1 -W 1 $addr &>/dev/null && return 0
    done
    echo "Can't ping $addr$action" >&2
    return 1
}

do_setup() {
    local action="$1" ip=$2
    if ! $SETUP "$@"; then
        echo "Failed to do $action on $ip" >&2
        return 1
    fi
}

configure() {
    do_setup reset $DEFAULT_IP "" || return 2
    check_ping $DEFAULT_IP 30 reset || return 3

    NORMAL_IP=10.24.$N.1
    NORMAL_MASK=$DEFAULT_MASK
    NORMAL_GW=10.24.$N.254
    do_setup change_ip $DEFAULT_IP "" \
        $NORMAL_IP $NORMAL_MASK $NORMAL_GW || return 4
    check_ping $NORMAL_IP 5 'address change' || return 5

    pwd=$(grep "^$N:" $(dirname $0)/hp_pwds | cut -f2 -d:)
    do_setup change_pwd $NORMAL_IP "" "$pwd" || return 6
    check_ping $NORMAL_IP 5 'password change' || return 7

    TEAM_VLAN=$((100+N))
    do_setup add_vlan $NORMAL_IP \
        "$pwd" $TEAM_VLAN "$(seq -s, 1 24)" || return 8
    check_ping $NORMAL_IP 5 'VLAN configuration' || return 9

    vlan_aware=23,24
    ingress_filter=$(seq -s, 1 22)
    tagged=
    pvids=$(for i in {1..22}; do echo -n ${TEAM_VLAN},; done; echo -n 1,1)
    do_setup configure_ports $NORMAL_IP "$pwd" \
        "$vlan_aware" "$ingress_filter" "$tagged" "$pvids" || return 10
    check_ping $NORMAL_IP 5 'port configuration' || return 11
}

ip addr add $DEFAULT_GW/$DEFAULT_PREFIX dev $DEV

check_ping $DEFAULT_IP 3 "" && up=true || up=false
if $up; then configure; configured=$?; fi

ip addr del $DEFAULT_GW/$DEFAULT_PREFIX dev $DEV

! $up && exit 20 || exit $configured
