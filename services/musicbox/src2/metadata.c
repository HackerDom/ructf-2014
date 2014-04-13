#include "metadata.h"

int mb_tag_init(char *key, char *value, struct Tag *tag) {
	char *new_key, *new_value;

	if ((new_key = (char *) malloc(strlen(key) + 1)) == NULL) {
		warn("Failed to allocate memory\n");
		return -1;
	}

	if ((new_value = (char *) malloc(strlen(value) + 1)) == NULL) {
		warn("Failed to allocate memory\n");
		free(new_key);
		return -1;
	}

	strcpy(new_key, key);
	strcpy(new_value, value);

	tag->key = new_key;
	tag->value = new_value;

	return 0;
}

int mb_tag_init_from_comment(char *comment, struct Tag *tag) {
	char *key, *value, *seppos;
	size_t key_length;

	if ((seppos = strchr(comment, '=')) == NULL) {
		warn("Wrong comment: '%s'", comment);
		return -1;
	}

	value = (char *) malloc(strlen(seppos + 1));
	strcpy(value, seppos + 1);
	key_length = seppos - comment;
	key = (char *)malloc(key_length + 1);
	memcpy(key, comment, key_length);
	key[key_length] = '\x00';

	if (mb_tag_init(key, value, tag) < 0)
		return -1;

	return 0;
};

char *mb_tag_to_str(struct Tag tag, int *length) {
	char *result;
	int key_length, result_length;

	key_length = strlen(tag.key);
	result_length = key_length + 1 + strlen(tag.value);
	*length = result_length;

	result = (char *) malloc(result_length + 1);

	strcpy(result, tag.key);
	strcpy(result + key_length + 1, tag.value);

	result[key_length] = '=';

	return result;
}

void mb_tag_clear(struct Tag *tag) {
	if (tag->key != NULL)
		free(tag->key);
	if (tag->value != NULL)
		free(tag->key);
	tag->key = NULL;
	tag->value = NULL;
}

int mb_read(void *buffer, size_t size, void **input, size_t *input_size) {
	int bytes = size;
	if (*input_size < bytes)
		bytes = *input_size;
	memcpy(buffer, *input, bytes);
	*input += bytes;
	*input_size -= bytes;
	return bytes;
}

int mb_write(void *buffer, size_t size, void **output) {
	memcpy(*output, buffer, size);
	*output += size;
	return size;
}

int mb_read_vorbis_headers(void **input, size_t *input_size, ogg_sync_state *oy, ogg_stream_state *os, vorbis_info *vi, vorbis_comment *vc) {
	ogg_page   og;
	ogg_packet op;

	char *buffer;
	int	bytes, i;

	ogg_sync_init(oy);

	buffer = ogg_sync_buffer(oy, READING_CHUNK_SIZE);
	bytes = mb_read(buffer, READING_CHUNK_SIZE, input, input_size);
	ogg_sync_wrote(oy, bytes);

	if (ogg_sync_pageout(oy, &og) != 1) {
		warn("Input does not appear to be an Ogg bitstream.\n");
		return -1;
	}

	ogg_stream_init(os, ogg_page_serialno(&og));

	vorbis_info_init(vi);
	vorbis_comment_init(vc);
	if (ogg_stream_pagein(os, &og) < 0) {
		warn("Error reading first page of Ogg bitstream data.\n");
		return -1;
	}

	if (ogg_stream_packetout(os, &op) != 1) {
		warn("Error reading initial header packet.\n");
		return -1;
	}

	if (vorbis_synthesis_headerin(vi, vc, &op) < 0) {
		warn("This Ogg bitstream does not contain Vorbis audio data.\n");
		return -1;
	}

	i = 0;
	while (i < 2) {
		while (i < 2) {
			int result = ogg_sync_pageout(oy, &og);
			if (result == 0)
				break;
			if (result == 1) {
				ogg_stream_pagein(os, &og);
				while (i < 2) {
					result = ogg_stream_packetout(os, &op);
					if (result == 0)
						break;
					if (result < 0) {
						warn("Corrupt secondary header.	Exiting.\n");
						return -1;
					}
					result = vorbis_synthesis_headerin(vi, vc, &op);
					if (result < 0) {
						warn("Corrupt secondary header.	Exiting.\n");
						return -1;
					}
					i++;
				}
			}
		}
		buffer = ogg_sync_buffer(oy, READING_CHUNK_SIZE);
		bytes = mb_read(buffer, READING_CHUNK_SIZE, input, input_size);

		if (bytes == 0 && i < 2) {
			warn("End of file before finding all Vorbis headers!\n");
			return -1;
		}
		ogg_sync_wrote(oy, bytes);
	}
}

int mb_set_metadata(void *input, size_t input_size, void *output, struct Tag *tags, size_t tags_size) {
	ogg_sync_state   oy;
	ogg_stream_state os;
	ogg_stream_state os_out;
	ogg_page         og;
	ogg_packet       op;

	vorbis_info      vi;
	vorbis_comment   vc;
	vorbis_dsp_state vd;

	int tag_length, new_comment_length, bytes, i, j, eos = 0, found;

	char *buffer, *new_comment;

	if (mb_read_vorbis_headers(&input, &input_size, &oy, &os, &vi, &vc) < 0) {
		warn("Failed to obtain metadata from file");
		return -1;
	}

	for (i = 0; i < tags_size; ++i) {
		tag_length = strlen(tags[i].key);
		new_comment = mb_tag_to_str(tags[i], &new_comment_length);

		found = 0;
		for (j = 0; j < vc.comments; ++j) {
			if (memcmp(tags[i].key, vc.user_comments[i], tag_length) == 0) {
				found = 1;
				free(vc.user_comments[i]);
				vc.user_comments[i] = new_comment;
				vc.comment_lengths[i] = new_comment_length;
				break;
			}
		}

		if (!found)
			vorbis_comment_add_tag(&vc, tags[i].key, tags[i].value);
	}

	debug("\n\n\n");
	for (i = 0; i < vc.comments; ++i) {
		debug("%s", vc.user_comments[i]);
	}
	debug("\n\n\n");

	vorbis_analysis_init(&vd,&vi);

	srand(time(NULL));
	ogg_stream_init(&os_out, rand());

	{
		ogg_packet header;
		ogg_packet header_comm;
		ogg_packet header_code;

		vorbis_analysis_headerout(&vd, &vc, &header, &header_comm, &header_code);
		ogg_stream_packetin(&os_out, &header);
		ogg_stream_packetin(&os_out, &header_comm);
		ogg_stream_packetin(&os_out, &header_code);

		while (!eos) {
			int result = ogg_stream_flush(&os_out, &og);
			if (result == 0)
				break;
			mb_write(og.header, og.header_len, &output);
			mb_write(og.body, og.body_len, &output);
		}
	}

	while (!eos) {
		while (!eos) {
			int result = ogg_sync_pageout(&oy, &og);
			if (result == 0)
				break;
			if (result < 0) {
				warn("Corrupt or missing data in bitstream; continuing...\n");
			} else {
				ogg_stream_pagein(&os, &og);
				while (1) {
					result = ogg_stream_packetout(&os, &op);
					
					if (result == 0)
						break;
					if (result > 0) {
						ogg_stream_packetin(&os_out, &op);

						while(!eos){
							int result = ogg_stream_pageout(&os_out, &og);
							if (result == 0)
								break;
							mb_write(og.header, og.header_len, &output);
							mb_write(og.body, og.body_len, &output);
						}
					}
				}
				if (ogg_page_eos(&og))
					eos = 1;
			}
		}
		if (!eos) {
			buffer = ogg_sync_buffer(&oy, READING_CHUNK_SIZE);
			bytes = mb_read(buffer, READING_CHUNK_SIZE, &input, &input_size);
			ogg_sync_wrote(&oy, bytes);
			if (bytes == 0)
				eos = 1;
		}
	}

	ogg_stream_clear(&os);
	ogg_stream_clear(&os_out);
	vorbis_dsp_clear(&vd);
	vorbis_comment_clear(&vc);
	vorbis_info_clear(&vi);
	ogg_sync_clear(&oy);

	return 0;
}

int mb_get_metadata(void *input, size_t input_size, struct Tag **tags, size_t *tags_count) {
	ogg_sync_state	 oy;
	ogg_stream_state os;
	ogg_page				 og;
	ogg_packet			 op;

	vorbis_info			vi;
	vorbis_comment	 vc;

	int i;

	if (mb_read_vorbis_headers(&input, &input_size, &oy, &os, &vi, &vc) < 0) {
		warn("Failed to obtain metadata from file");
		return -1;
	}

	*tags = (struct Tag *) malloc(sizeof(struct Tag) * vc.comments);
	*tags_count = 0;

	for (i = 0; i < vc.comments; ++i) {
		size_t index = (*tags_count)++;
		struct Tag *tag = *tags + index;
		if (mb_tag_init_from_comment(vc.user_comments[i], tag) < 0) {
			continue;
		}
	}

	*tags = (struct Tag *) realloc(*tags, sizeof(struct Tag) * (*tags_count));

	ogg_stream_clear(&os);
	vorbis_comment_clear(&vc);
	vorbis_info_clear(&vi);
	ogg_sync_clear(&oy);

	return 0;
}