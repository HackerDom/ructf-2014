#!/bin/bash

BASE=$(dirname $0)

make -j4 -C$BASE/md && {
    { while true; do $BASE/mon_iptables.pl; sleep 1; done } | \
    $BASE/md/md | \
    $BASE/mon_format.py
}
