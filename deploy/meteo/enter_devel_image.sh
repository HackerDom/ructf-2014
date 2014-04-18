#!/bin/bash

service_name=meteo

docker start ${service_name}_mysql
docker run -it -p 5001:5000 --link ${service_name}_mysql:db --entrypoint=/bin/bash ructf2014:${service_name}_service -c /bin/bash
