#pragma once

int user_create(char *user, char *pass);

int rooms_get(char **rooms, int size);

int room_join(char *name, int userId, char *pass);

int leave(int roomId, int userId);
