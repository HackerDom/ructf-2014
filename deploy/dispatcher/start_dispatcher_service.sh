#!/bin/bash


docker run --cidfile=/var/run/dispatcher_service.cont_id --volumes-from dispatcher_data -d -p 5000:5000 ructf2014:dispatcher_service
