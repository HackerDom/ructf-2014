#!/bin/bash

docker run -it -p 5000:5000 --entrypoint=/bin/bash ructf2014:dispatcher_service -c /bin/bash
