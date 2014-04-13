#ifndef MUSICBOX_SERVER_H
#define MUSICBOX_SERVER_H

#include "common.h"
#include "logging.h"

enum MBRequestType {
	MB_REQUEST_PUT,
	MB_REQUEST_GET
};

enum MBResponseType {
	MB_RESPONSE_SUCCESS,
	MB_RESPONSE_ERROR
};

void mb_start_server(int port, struct Store *store);

#endif