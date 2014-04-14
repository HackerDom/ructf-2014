#!/bin/bash

for line in $(cat $(dirname $0)/hp_pwds); do
    n=${line%%:*}
    $(dirname $0)/do_setup_hp.sh eth0.$((200+n)) $n
done
