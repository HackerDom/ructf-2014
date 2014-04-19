#!/bin/bash

# change directory to the script location
cd "$( dirname "${BASH_SOURCE[0]}")"

chat/build_chat_mongodb.sh
chat/build_chat_service.sh

dispatcher/build_dispatcher_data.sh
dispatcher/build_dispatcher_service.sh

feedback/build_feedback_data.sh
feedback/build_feedback_service.sh

FMGS/build_FMGS_data.sh
FMGS/build_FMGS_service.sh

meteo/build_meteo_mysql.sh
meteo/build_meteo_collector.sh
meteo/build_meteo_service.sh
