#!/bin/bash

service_name=dispatcher

docker run -it -p 5001:5000 --volumes-from ${service_name}_data ructf2014:${service_name}_service /bin/bash
