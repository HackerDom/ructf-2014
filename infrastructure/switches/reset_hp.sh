#!/bin/bash

for line in $(cat $(dirname $0)/hp_pwds); do
    n=${line%%:*}
    pwd=${line##*:}
    python $(dirname $0)/setup_hp.py reset 10.24.$n.1 $pwd;
done
