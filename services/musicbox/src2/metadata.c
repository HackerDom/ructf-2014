#include "metadata.h"

struct Tag mb_tag_init(char *key, char *value) {
	struct Tag tag;

	tag.key = key;
	tag.value = value;

	return tag;
}

int mb_tag_init(char *comment, struct Tag *tag) {
	char *key, *value, *seppos;
	size_t key_length;

	if ((seppos = strchr(comment, '=')) == NULL) {
		char buffer[strlen(comment) + 32];
		sprintf(buffer, "Wrong comment: '%s'", comment);
		info(buffer);
		return -1;
	}

	value= = (char *) malloc(strlen(seppos + 1));
	strcpy(value, seppos + 1);
	key_length = seppos - comment;
	key = (char *)malloc(key_length + 1);
	memcpy(key, comment, key_length);
	key[key_length] = '\x00';

	*tag = mb_tag_init(key, value);

	return 0;
};

char *mb_tag_to_str(struct Tag tag, int *length) {
	char *result;
	int key_length, result_length;

	key_length = strlen(tag.key);
	result_length = key_length + 1 + strlen(tag.value);
	if (length != NULL)
		length = result_length;

	result = (char *) malloc(result_length + 1);

	strcpy(result, tag.key);
	strcpy(result + key_length + 1, tag.value);

	result[key_length] = '=';

	return result;
}

void mb_tag_clear(struct Tag *tag) {
	if (*tag.key != NULL)
		free(*tag, key);
	if (*tag.value != NULL)
		free(*tag, key);
	*tag.key = NULL;
	*tag.value = NULL;
}

int mb_set_metadata(FILE *input, FILE *output, struct Tag *tags, int tags_count) {
	OggVorbis_File *vf;
	vorbis_comment *vc;
	int tag_length, new_comment_length;
	bool found;

	if (ov_open(input, vf, NULL, 0) < 0) {
		info("Failed to open file as vorbis");
		return -1;
	}

	vc = vf.vc;

	for (i = 0; i < tags_count; ++i) {
		tag_length = strlen(tags[i].key);
		new_comment = mb_tag_to_str(tags[i], &new_comment_length);

		found = false;
		for (j = 0; j < vc.comments; ++j) {
			if (memcmp(tags[i].key, vc.user_comments[i], tag_length) == 0) {
				found = true;
				free(vc.user_comments[i]);
				vc.user_comments[i] = new_comment;
				vc.comment_lengths[i] = new_comment_length;
				break;
			}
		}

		if (!found)
			vorbis_comment_add_tag(vc, tags[i].key, tags[i].value);
	}

	return 0;
}

int mb_get_metadata(FILE *input, struct Tag **tags, int *tags_count) {
	OggVorbis_File *vf;
	vorbis_comment *vc;
	int i;
	Tag tag;

	if (ov_open(input, vf, NULL, 0) < 0) {
		info("Failed to open file as vorbis");
		return -1;
	}

	vc = vf.vc;

	*tags = (Tag *) malloc(sizeof(struct Tag) * vc.comments);
	*tags_count = 0;

	for (i = 0; i < vc.comments; ++i) {
		if (mb_tag_init(vc.user_comments[i], &tag) < 0)
			continue;
		*tags[++(*tags_count)] = tag;
	}

	*tags = (Tag *) realloc(*tags, sizeof(struct Tag) * (*tags_count));

	return 0;
}