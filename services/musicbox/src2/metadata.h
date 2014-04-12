#ifndef MUSICBOX_METADATA_H
#define MUSICBOX_METADATA_H

#include "common.h"
#include "logging.h"

struct Tag {
	char *key;
	char *value;
};

// Returns new Tag structure with 'key' and 'value' fields set according to parameters

struct Tag mb_tag_init(char *key, char *value);

// Sets tag to new Tag structure, with NEW 'key' and 'value', generated from Vorbis comment string
// Returns 0 on success and -1 when comment string is invalid

int mb_tag_init(char *comment, struct Tag *tag);

// Creates NEW string, which has to be deleted outside

char *mb_tag_to_str(struct Tag tag);

// Frees Tag structure (frees key and value)

void mb_tag_clear(struct Tag *tag);

int mb_set_metadata(void *input, size_t input_size, void *output, struct Tag *tags, size_t tags_size);

int mb_get_metadata(void *input, size_t input_size, struct Tag **tags);

#endif