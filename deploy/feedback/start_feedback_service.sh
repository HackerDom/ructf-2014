#!/bin/bash

service_name=feedback

docker run --memory=500m --cpu-shares=10 --cidfile=/var/run/${service_name}_service.cont_id --volumes-from ${service_name}_data -d -p 7654:7654 ructf2014:${service_name}_service
