#!/bin/bash

# change directory to the script location
cd "$( dirname "${BASH_SOURCE[0]}")"

docker build -t ructf2014:simple_data simple_data

echo "Creating named container, please delete old one if you want"
docker run --name simple_data ructf2014:simple_data chown -R simple:simple /home/simple/
