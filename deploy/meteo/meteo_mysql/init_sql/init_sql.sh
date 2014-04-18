#!/usr/bin/bash

mysqld_safe &

echo Trying to init the database

while ! mysql -h 127.0.0.1 < /tmp/init_sql/init.sql; do
 sleep 1
done
