#!/bin/bash

service_name=meteo

# change directory to the script location
cd "$( dirname "${BASH_SOURCE[0]}")"

docker build -t ructf2014:${service_name}_mysql ${service_name}_mysql

echo "Creating named container, please delete old one if you want"
docker run --memory=500m --cpu-shares=10 -d --name ${service_name}_mysql ructf2014:${service_name}_mysql
docker stop meteo_mysql
