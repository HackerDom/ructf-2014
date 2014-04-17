#!/bin/bash

BASE=$(dirname $0)

{ while true; do $BASE/mon_iptables.pl; sleep 1; done } | $BASE/md/md
