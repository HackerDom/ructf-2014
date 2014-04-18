#!/bin/bash

service_name=FMGS

docker run -it -p 44101:44100 --volumes-from ${service_name}_data --entrypoint=/bin/bash ructf2014:${service_name}_service -c /bin/bash
