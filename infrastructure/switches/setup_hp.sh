#!/bin/bash

DEV=${1?no device}
N=${2?no team number}

SETUP="python $(dirname $0)/setup_hp.py"

DEFAULT_IP=192.168.2.10
DEFAULT_MASK=255.255.255.0
DEFAULT_PREFIX=24
DEFAULT_GW=192.168.2.1

ip addr add $DEFAULT_GW/$DEFAULT_PREFIX dev $DEV

configure() {
    if ! $SETUP reset $DEFAULT_IP ""; then
        echo "Failed to reset $DEFAULT_IP" >&2
        return 2
    fi
    local ping=false
    for ((i=0; i!=30; ++i)); do
        if ping -c 1 -W 1 $DEFAULT_IP &>/dev/null; then
            ping=true
            break
        fi
    done
    if ! $ping; then
        echo "Can't ping $DEFAULT_IP after reset" >&2
        return 3
    fi

    NORMAL_IP=10.24.$N.1
    NORMAL_MASK=$DEFAULT_MASK
    NORMAL_GW=10.24.$N.254
    if ! $SETUP change_ip $DEFAULT_IP "" \
            $NORMAL_IP $NORMAL_MASK $NORMAL_GW; then
        echo "Failed to change ip to $NORMAL_IP" >&2
        return 4
    fi

    ping=false
    for ((i=0; i!=5; ++i)); do
        if ping -c 1 -W 1 $NORMAL_IP &>/dev/null; then
            ping=true
            break
        fi
    done
    if ! $ping; then
        echo "Can't ping $NORMAL_IP after address change" >&2
        return 5
    fi

    pwd=$(grep "^$N:" $(dirname $0)/hp_pwds | cut -f2 -d:)
    if ! $SETUP change_pwd $NORMAL_IP "" $pwd; then
        echo "Can't change pwd on $NORMAL_IP" >&2
        return 6
    fi

    #TODO(malets): VLANs, ports, ...
}

succ=false
for ((i=0; i!=3; ++i)); do
    if ping -c 1 -W 1 $DEFAULT_IP &>/dev/null; then
        succ=true
        configure
        configured=$?
        break
    fi
done

ip addr del $DEFAULT_GW/$DEFAULT_PREFIX dev $DEV

if ! $succ; then
    echo "Can't ping $DEFAULT_IP" >&2
    exit 1
fi

if ! $configure; then
    echo "Can't configure $DEFAULT_IP" >&2
    exit 2
fi
