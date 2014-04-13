#pragma once

#include <stdbool.h>

int db_connect();
void db_disconnect();

int user_create(char *user, char *pass);
int user_login(char *user, char *pass);
int room_create(char *name, char *pass);
int list();
const char *user_name(const char *userId);
int list_room();
int say( char *message);
int room_join(char *name, char *pass);
