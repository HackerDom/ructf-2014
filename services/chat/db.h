#pragma once

#include <stdbool.h>

int  db_connect();
void db_disconnect();

void user_create(char *user, char *pass);
void user_login(char *user, char *pass);

void room_create(char *name, char *pass);
void room_join(char *name, char *pass);
void room_history();
void signal_handler();
bool room_one_msg();
void room_leave();
void set_signal();
void prompt();
int list();
void say(char *message);

const char *user_name(const char *userId);
const char *room_name(const char *roomId);
