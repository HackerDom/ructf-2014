#include "storage.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "debug.h"
#include "db.h"

#define CHECK(f,name) if (!f) {D("Cannot open file: %s", name);return -1;}

#define FILE_USERS "users.db"
#define FILE_ROOMS "rooms.db"

int append(char *fname, char *format, ...)
{
    va_list args;

    FILE * f = fopen(fname, "a");
    CHECK(f, fname);

    va_start(args, format);
    vfprintf(f, format, args);
    va_end(args);
    fclose(f);
}

int lines_count(char *fname)
{
    char buf[1024];

    FILE * f = fopen(fname, "r");
    CHECK(f, fname);

    int result = 0;
    while (!feof(f)) {
        fgets(buf, 1024, f);
        result++;
    }
    fclose(f);

    return result;
}

int rooms_get(char **rooms, int size)
{
    D("rooms_get\r\n");
    if (size > 0)
        strcpy(rooms[0], "room one");
    if (size > 1)
        strcpy(rooms[1], "room two");
    return 0;
}


int leave(int roomId, int userId)
{
    D("leave: %d, %d")
    return 0;
}
