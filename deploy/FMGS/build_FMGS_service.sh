#!/bin/bash

service_name=FMGS

# change directory to the script location
cd "$( dirname "${BASH_SOURCE[0]}")"

docker build -t ructf2014:${service_name}_service ${service_name}_service
