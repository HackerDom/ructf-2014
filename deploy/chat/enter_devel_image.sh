#!/bin/bash

service_name=chat

docker start ${service_name}_mongodb
docker run  --memory=500m --cpu-shares=10 -it -p 5556:5555 --link ${service_name}_mongodb:db ructf2014:${service_name}_service /bin/bash
