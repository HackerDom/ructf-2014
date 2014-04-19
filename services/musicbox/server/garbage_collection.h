#ifndef MUSICBOX_GARBAGE_COLLECTION_H
#define MUSICBOX_GARBAGE_COLLECTION_H

#include "metadata.h"
#include <sys/stat.h>
#include <dirent.h>

int mb_collect(char *dirname);

size_t mb_occupied_space(char *dirname);

#endif