#!/bin/bash

service_name=feedback

docker run --memory=500m --cpu-shares=10 -it -p 7655:7654 --volumes-from ${service_name}_data ructf2014:${service_name}_service /bin/bash
