#!/bin/bash

mpd --no-daemon &
icecast -c /etc/icecast.xml &

while true; do 
    mpc crop
    mpc ls | mpc add

    sleep 5
wait