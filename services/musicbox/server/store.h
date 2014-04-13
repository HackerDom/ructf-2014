#ifndef MUSICBOX_STORE_H
#define MUSICBOX_STORE_H

#include "common.h"

struct Store {
	char *dir;
};

void mb_store_init(struct Store *store, const char *dir);

void mb_store_clear(struct Store *store);

int mb_store_put(struct Store *store, uuid_t id, uint8_t *buffer, int length);

int mb_store_get(struct Store *store, uuid_t id, uint8_t *buffer, int length);

#endif