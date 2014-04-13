#pragma once

char currentUser[25];
char currentRoom[25];

int connect_db();
void disconnect_db();

int add_user(char *user, char *pass);
int user_login(char *user, char *pass);
int room_create(char *name, char *ownerId, char *pass);
int list();
int list_room();
int say( char *message);
int room_join(char *name, char *pass);
