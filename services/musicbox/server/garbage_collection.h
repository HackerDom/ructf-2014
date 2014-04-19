#ifndef MUSICBOX_GARBAGE_COLLECTION_H
#define MUSICBOX_GARBAGE_COLLECTION_H

#include "metadata.h"
#include <sys/stat.h>
#include <dirent.h>

int collect(char *dirname);

size_t occupied_space(char * dirname);

#endif