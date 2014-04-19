#!/bin/bash

echo Starting docker daemon
screen -d -m -L docker -d
sleep 15

echo Starting FMGS service
/home/FMGS/start_FMGS_service.sh
sleep 10

echo Starting chat service
/home/chat/start_chat_service.sh
sleep 10

echo Starting dispatcher service
/home/dispatcher/start_dispatcher_service.sh
sleep 10

echo Starting feedback service
/home/feedback/start_feedback_service.sh
sleep 10

echo Starting meteo service
/home/meteo/start_meteo_mysql.sh
sleep 5
/home/meteo/start_meteo_collector.sh
sleep 5
/home/meteo/start_meteo_service.sh
sleep 10

echo Starting musicbox service
/home/musicbox/start_musicbox_radio.sh
sleep 5
/home/musicbox/start_musicbox_service.sh
