#include "garbage_collection.h"

int mb_has_to_be_removed(char *filename) {
	struct Tag *tags;
	size_t tags_count, file_size, i;
	int ttl, result;
	FILE *f;
	uint8_t *buffer;
	time_t current_time;
	struct stat file_stat;

	if ((f = fopen(filename, "rb")) == NULL) {
		warn("Failed to open file %s", filename);
		return 1;
	}

	if (stat(filename, &file_stat) < 0) {
		warn("Failed to get stat of file %s", filename);
		return 1;
	}

	fseek(f, 0, SEEK_END);
	file_size = ftell(f);
	fseek(f, 0, SEEK_SET);
	buffer = (uint8_t *) malloc(file_size);
	if (buffer == NULL) {
		warn("Failed to allocate memory for buffer");
		return 1;
	}
	fread(buffer, 1, file_size, f);

	if (mb_get_metadata(buffer, file_size, &tags, &tags_count) < 0) {
		free(buffer);
		warn("Failed to obtain metadata of file %s", filename);
		return 1;
	}

	current_time = time(NULL);

	result = 1;

	for (i = 0; i < tags_count; ++i) {
		if (strcmp(tags[i].key, "TTL") == 0) {
			ttl = strtol(tags[i].value, NULL, 0);
			if (file_stat.st_mtime + ttl > current_time) {
				result = 0;
				break;
			}
		}
	}

	free(buffer);
	free(tags);

	return result;
}

void mb_collect_file(char *filename) {
	if (mb_has_to_be_removed(filename)) {
		info("Removing %s", filename);
		if (remove(filename) < 0) {
			warn("Failed to remove file %s", filename);
		}
	}
}

int mb_collect(char *dirname) {
	struct dirent **files;
	int files_count, i;

	if ((files_count = scandir(dirname, &files, NULL, alphasort)) < 0) {
		warn("Failed to obtain listing of directory %s", dirname);
		return -1;
	}

	for (i = 0; i < files_count; ++i) {
		mb_collect_file(files[i]->d_name);
	}

	free(files);
}

size_t mb_get_file_size(char *filename) {
	struct stat file_stat;

	if (stat(filename, &file_stat) < 0) {
		warn("Failed to get stat of file %s", filename);
		return 0;
	}

	return file_stat.st_size;
}

size_t mb_occupied_space(char * dirname) {
	struct dirent **files;
	int files_count, i;
	size_t total_size;

	if ((files_count = scandir(dirname, &files, NULL, alphasort)) < 0) {
		warn("Failed to obtain listing of directory %s", dirname);
		return -1;
	}

	total_size = 0;
	for (i = 0; i < files_count; ++i) {
		total_size += mb_get_file_size(files[i]->d_name);
	}

	free(files);

	return total_size;
}