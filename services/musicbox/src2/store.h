#ifndef MUSICBOX_STORE_H
#define MUSICBOX_STORE_H

#include "common.h"

struct Store {
	char *folder;
};

int mb_store_init(struct Store *store, char *folder);

int mb_store_put(struct Store *store, uuid_t *id, uint_8 *buffer, int length);

int mb_store_get(struct Store *store, uuid_t id, uint_8 *buffer, int *length);

#endif