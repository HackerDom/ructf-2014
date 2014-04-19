#!/bin/bash

service_name=meteo

docker start ${service_name}_mysql
docker run --memory=500m --cpu-shares=10 -it -p 9001:9000 --link ${service_name}_mysql:db ructf2014:${service_name}_collector /bin/bash
