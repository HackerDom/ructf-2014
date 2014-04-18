#!/bin/bash

service_name=meteo

docker start ${service_name}_mysql
docker run -it -p 8889:8888 --link ${service_name}_mysql:db ructf2014:${service_name}_service /bin/bash
