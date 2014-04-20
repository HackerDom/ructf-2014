#!/bin/bash

service_name=musicbox

docker run --memory=500m --cpu-shares=10 --cidfile=/var/run/${service_name}_service.cont_id --volumes-from ${service_name}_data -d -p 4242:4242 -p 16780:16780 ructf2014:${service_name}_service
