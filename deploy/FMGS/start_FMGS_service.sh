#!/bin/bash

service_name=FMGS

docker run --cidfile=/var/run/${service_name}_service.cont_id --volumes-from ${service_name}_data -d -p 44100:44100 ructf2014:${service_name}_service
