#!/bin/bash

service_name=musicbox

docker run --memory=500m --cpu-shares=10 -it -p 4243:4242 -p 16781:16780 --volumes-from ${service_name}_data ructf2014:${service_name}_service /bin/bash
