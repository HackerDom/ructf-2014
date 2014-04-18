#!/bin/bash

service_name=chat

# change directory to the script location
cd "$( dirname "${BASH_SOURCE[0]}")"

docker build -t ructf2014:${service_name}_mongodb ${service_name}_mongodb

echo "Creating named container, please delete old one if you want"
docker run -d --name ${service_name}_mongodb ructf2014:${service_name}_mongodb
docker stop ${service_name}_mongodb
