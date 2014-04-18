#!/bin/bash

service_name=meteo

docker start ${service_name}_mysql
docker run -it -p 9001:9000 --link ${service_name}_mysql:db ructf2014:${service_name}_collector /bin/bash
