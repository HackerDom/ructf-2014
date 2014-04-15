#ifndef MUSICBOX_COMMON_H
#define MUSICBOX_COMMON_H

#define MAX_FILE (1 * 1024 * 1024)
#define MAX_CHUNK 0x7fff
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
#include <netinet/in.h>

#include <vorbis/vorbisfile.h>

#endif