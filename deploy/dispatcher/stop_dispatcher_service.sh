#!/bin/bash

#!/bin/bash

if [ ! -f  /var/run/dispatcher_service.cont_id ]; then
 echo "Service is not running"
 exit 1
fi

id=$(cat /var/run/dispatcher_service.cont_id)
docker stop "${id}" && rm /var/run/dispatcher_service.cont_id
