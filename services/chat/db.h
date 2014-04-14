#pragma once

#include <stdbool.h>

int  db_connect();
void db_disconnect();

void user_create(char *user, char *pass);
void user_login(char *user, char *pass);

void room_create(char *name, char *pass);
void room_join(char *name, char *pass);
void room_history();

int list();
void say(char *message);

const char *user_name(const char *userId);


