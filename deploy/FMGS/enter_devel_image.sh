#!/bin/bash

service_name=FMGS

docker run --memory=500m --cpu-shares=10 -it -p 44101:44100 --volumes-from ${service_name}_data ructf2014:${service_name}_service /bin/bash
