#!/bin/bash

mpd --no-daemon &
icecast -c /etc/icecast.xml &

wait