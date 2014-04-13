#ifndef MUSICBOX_METADATA_H
#define MUSICBOX_METADATA_H

#include "common.h"
#include "logging.h"

struct Tag {
	char *key;
	char *value;
};

#define READING_CHUNK_SIZE ((size_t) 4096)

// Returns new Tag structure with 'key' and 'value' fields set according to parameters

int mb_tag_init(char *key, char *value, struct Tag *tag);

// Creates NEW string, which has to be deleted outside

void mb_tag_clear(struct Tag *tag);

int mb_set_metadata(void *input, size_t input_size, void *output, struct Tag *tags, size_t tags_size);

int mb_get_metadata(void *input, size_t input_size, struct Tag **tags, size_t *tags_count);

#endif