#pragma once

int connect_db();
int add_user(char *user, char *pass);
int destrot_db();
int user_login(char *user, char *pass);
int room_create(char *name, int ownerId, char *pass);
