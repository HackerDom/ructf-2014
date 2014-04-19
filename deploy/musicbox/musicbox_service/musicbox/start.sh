#!/bin/bash

./musicbox &

cd www
./server.py &

wait
