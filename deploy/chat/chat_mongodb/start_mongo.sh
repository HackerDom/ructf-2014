#!/bin/bash

/usr/sbin/mongod --dbpath /data/db --repair
/usr/sbin/mongod --dbpath /data/db
