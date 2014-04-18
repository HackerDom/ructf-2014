#!/bin/bash

service_name=feedback

docker run -it -p 7655:7654 --volumes-from ${service_name}_data ructf2014:${service_name}_service /bin/bash
