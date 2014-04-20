#!/bin/bash

mpd --no-daemon &
icecast -c /etc/icecast.xml &

mpc random
mpc repeat

while true; do 
    mpc crop
    mpc ls | mpc add

    mpc play
    sleep 5
done
wait
