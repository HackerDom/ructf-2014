#!/bin/bash

service_name=meteo

# change directory to the script location
cd "$( dirname "${BASH_SOURCE[0]}")"

docker build -t ructf2014:${service_name}_collector ${service_name}_collector
