#!/bin/bash

service_name=dispatcher

docker run --memory=500m --cpu-shares=10 -it -p 5001:5000 --volumes-from ${service_name}_data ructf2014:${service_name}_service /bin/bash
