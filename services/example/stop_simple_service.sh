#!/bin/bash

if [ ! -f  /var/run/simple_service.cont_id ]; then
 echo "Service is not running"
 exit 1
fi

id=$(cat /var/run/simple_service.cont_id)
docker stop "${id}" && rm /var/run/simple_service.cont_id
