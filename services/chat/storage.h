#pragma once

int user_create(char *user, char *pass);

int user_login(char *user, char *pass);

int rooms_get(char **rooms, int size);

int room_create(char *name, int ownerId, char *pass);

int room_join(int roomId, int userId, char *pass);

int say(int roomId, int userId, char *message);

int leave(int roomId, int userId);
