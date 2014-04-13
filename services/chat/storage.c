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

int user_create(char *user, char *pass)
{
    D("user_create: user=%s, pass=%s", user, pass);
	
    if (append(FILE_USERS, "%s:%s", user, pass) < 0)
        return -1;
    D("/nuser")
    add_user(user,pass);
	int userId = lines_count(FILE_USERS);
    D("  OK, assigned userId=%d", userId);

    return userId;
}

int user_login(char *user, char *pass)
{
    D("user_login: %s, %s", user, pass);
    return 0;
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

int room_create(char *name, int ownerId, char *pass)
{
    D("room_create: %s, %d, %s", name, ownerId, pass);

    append(FILE_ROOMS, "%s:%d:%s", name, ownerId, pass);

    return 0;
}

int room_join(char *name, int userId, char *pass)
{
    D("room_join: name=%s, userId=%d, pass=%s", name, userId, pass);

    return 0;
}

int say(int roomId, int userId, char *message)
{
    D("say: %d, %d, %s", roomId, userId, message);
    return 0;
}

int leave(int roomId, int userId)
{
    D("leave: %d, %d")
    return 0;
}
