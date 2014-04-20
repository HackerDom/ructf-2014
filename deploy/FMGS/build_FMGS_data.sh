#!/bin/bash

service_name=FMGS

# change directory to the script location
cd "$( dirname "${BASH_SOURCE[0]}")"

docker build -t ructf2014:${service_name}_data ${service_name}_data

echo "Creating named container, please delete old one if you want"
docker run --name ${service_name}_data ructf2014:${service_name}_data chown -R fmgs:fmgs /home/${service_name}/
