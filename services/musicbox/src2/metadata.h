#ifndef MUSICBOX_METADATA_H
#define MUSICBOX_METADATA_H

#include <common.h>

struct Tag {
	char *key;
	char *value;
};

Tag mb_tag_init(char *key, char *value);

int mb_set_metadata(Tag *tags, int tags_count);

int mb_get_metadata(Tag **tags);

#endif