#ifndef MUSICBOX_NETWORKING_H
#define MUSICBOX_NETWORKING_H

#include <common.h>

int mb_send_all_bytes(int sockfd, uint_8 *buffer, int length);

int mb_recieve_all_bytes(int sockfd, uint_8 *buffer, int length);

#endif