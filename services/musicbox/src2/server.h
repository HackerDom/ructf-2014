#ifndef MUSICBOX_SERVER_H
#define MUSICBOX_SERVER_H

#include "common.h"
#include "logging.h"

enum MBRequestType {
	MB_REQUEST_PUT,
	MB_REQUEST_GET
};

enum MBResponseType {
	MB_RESPONSE_PUT_OK,
	MB_RESPONSE_GET_OK,
	MB_RESPONSE_UNKNOWN_REQUEST_TYPE_ERROR
};

void mb_start_server(int port);

void mb_process_client(int sockfd);

#endif