#!/bin/bash

service_name=meteo

docker start ${service_name}_mysql
docker run --memory=500m --cpu-shares=10 --cidfile=/var/run/${service_name}_service.cont_id --link ${service_name}_mysql:db -d -p 8888:8888 ructf2014:${service_name}_service
