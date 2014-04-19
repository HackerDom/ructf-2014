#!/bin/bash

service_name=meteo

if [ ! -f  /var/run/${service_name}_collector.cont_id ]; then
 echo "Service is not running"
 exit 1
fi

id=$(cat /var/run/${service_name}_collector.cont_id)
docker stop "${id}" && rm /var/run/${service_name}_collector.cont_id
