#!/bin/bash

service_name=dispatcher

docker run -it -p 5000:5001 --volumes-from dispatcher_data --entrypoint=/bin/bash ructf2014:${service_name}_service -c /bin/bash
