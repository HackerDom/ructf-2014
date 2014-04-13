#pragma once

int user_create(char *user, char *pass);

int rooms_get(char **rooms, int size);

int leave(int roomId, int userId);
