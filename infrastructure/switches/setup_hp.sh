#!/bin/bash -x

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
    local reset=false
    for ((i=0; i!=30; ++i)); do
        if ping -c 1 -W 1 $DEFAULT_IP &>/dev/null; then
            reset=true
            break
        fi
    done
    if ! $reset; then
        echo "Can't ping $DEFAULT_IP after reset" >&2
        return 3
    else
        NORMAL_IP=10.24.$N.1
        NORMAL_MASK=$DEFAULT_MASK
        NORMAL_GW=10.24.$N.254
        if ! $SETUP change_ip $DEFAULT_IP "" \
                $NORMAL_IP $NORMAL_MASK $NORMAL_GW; then
            echo "Failed to change ip to $NORMAL_IP" >&2
            return 4
        fi
    fi
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
