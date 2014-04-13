#ifndef MUSICBOX_NETWORKING_H
#define MUSICBOX_NETWORKING_H

#include "common.h"

int mb_send_all_bytes(int sockfd, uint8_t *buffer, int length);

int mb_receive_all_bytes(int sockfd, uint8_t *buffer, int length);

#endif