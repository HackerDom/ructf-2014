#!/bin/bash


docker run --cidfile=/var/run/simple_service.cont_id -d -p 10000:10000 --volumes-from simple_data ructf2014:simple_service

