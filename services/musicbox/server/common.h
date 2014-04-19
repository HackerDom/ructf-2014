#ifndef MUSICBOX_COMMON_H
#define MUSICBOX_COMMON_H

#define MAX_FILE (1 * 1024 * 1024)
#define MAX_CHUNK 0x7fff
#define MAX_TTL 5400
#define MAX_STORAGE_SPACE (1 * 1024 * 1024 * 1024)
#define min(a, b) ((a) <= (b) ? (a) : (b))

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#include <uuid/uuid.h>

#include <sys/time.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>

#include <vorbis/vorbisfile.h>

#endif