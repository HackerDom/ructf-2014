#!/bin/bash

service_name=chat

docker start ${service_name}_mongodb
docker run --cidfile=/var/run/${service_name}_service.cont_id --link ${service_name}_mongodb:db -d -p 5555:5555 ructf2014:${service_name}_service
