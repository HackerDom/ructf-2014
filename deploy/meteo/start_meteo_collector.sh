#!/bin/bash

service_name=meteo

docker start ${service_name}_mysql
docker run --memory=500m --cpu-shares=10 --cidfile=/var/run/${service_name}_collector.cont_id --link ${service_name}_mysql:db -d -p 9000:9000 ructf2014:${service_name}_collector
