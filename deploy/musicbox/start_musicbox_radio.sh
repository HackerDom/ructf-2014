#!/bin/bash

service_name=musicbox

docker run --memory=500m --cpu-shares=10 --cidfile=/var/run/${service_name}_radio.cont_id --volumes-from ${service_name}_data -d -p 17216:17216 ructf2014:${service_name}_radio
