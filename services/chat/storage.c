#include "storage.h"
#include "debug.h"

int user_create(char *user, char *pass)
{
    D("user_create: %s, %s", user, pass);
    return 0;
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
        strcpy(rooms[1], "room two")
    return 0;
}

int room_create(char *name, int ownerId, char *pass)
{
    D("room_create: %s, %d, %s", name, ownerId, pass);
    return 0;
}

int room_join(int roomId, int userId, char *pass)
{
    D("room_join: %d, %d, %s", roomId, ownerId, pass);
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
