#!/bin/bash

service_name=musicbox

docker run --memory=500m --cpu-shares=10 -it -p 17217:17216 --volumes-from ${service_name}_data ructf2014:${service_name}_radio /bin/bash
