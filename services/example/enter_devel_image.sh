#!/bin/bash

docker run -it -p 10001:10000 --entrypoint=/bin/bash --volumes-from simple_data ructf2014:simple_service -c /bin/bash
