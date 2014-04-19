#include "store.h"
#include "logging.h"
#include "garbage_collection.h"
#include "common.h"

void mb_store_init(struct Store *store, const char *dir) {
	store->dir = strdup(dir);
}

void mb_store_clear(struct Store *store) {
	free(store->dir);
}

void mb_replace(char *str, char replace_from, char replace_to) {
	int i, length;

	length = strlen(str);

	for (i = 0; i < length; ++i) {
		str[i] = str[i] == replace_from 
			? replace_to 
			: str[i];
	}
}

int mb_store_put(struct Store *store, uuid_t id, uint8_t *buffer, int length) {
	char id_text[37];
	char path[256];
	FILE *file;
	size_t bytes_written;

	uuid_generate_random(id);
	uuid_unparse(id, id_text);
	mb_replace(id_text, '-', '_');
	sprintf(path, "%s/%s.ogg", store->dir, id_text);
	file = fopen(path, "w");
	if (file == NULL) {
		warn("Failed to open location %s", path);
		return -1;
	}
	bytes_written = fwrite(buffer, 1, length, file);
	if (bytes_written != length)
		return -1;
	fclose(file);

	if (mb_occupied_space(store->dir) > MAX_STORAGE_SPACE)
		mb_collect(store->dir);

	return bytes_written;
}

int mb_store_get(struct Store *store, uuid_t id, uint8_t *buffer, int capacity) {
	char id_text[37];
	char path[256];
	FILE *file;
	int length;

	uuid_unparse(id, id_text);
	mb_replace(id_text, '-', '_');
	sprintf(path, "%s/%s.ogg", store->dir, id_text);
	debug("mb_store_get: %s", path);
	file = fopen(path, "r");
	if (file == NULL)
		return -1;
	debug("mb_store_get: open successful");
	length = fread(buffer, 1, capacity, file);
	fclose(file);
	return length;
}