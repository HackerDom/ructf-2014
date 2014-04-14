#include "db.h"
#include "mongo.h"
#include "io.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "debug.h"

#define MONGO_HOST "127.0.0.1"
#define MONGO_PORT 27017

#define COLLECTION_USERS     "chat.users"
#define COLLECTION_ROOMS     "chat.rooms"
#define COLLECTION_MESSAGES  "chat.msg"

bool userSet = false;
bool roomSet = false;
char currentUser[25];
char currentRoom[25];

mongo conn;

int db_connect()
{
    int status = mongo_client( &conn, MONGO_HOST, MONGO_PORT );
    return status == MONGO_OK ? 0 : -1;
}

void db_disconnect()
{
    mongo_destroy(&conn);
}

bool exists(char *collection, char *name, char *value)
{
    bson b;
    mongo_cursor cursor;
    bson_iterator it;

    bson_init( &b );
    bson_append_string( &b, name, value);
    bson_finish( &b );

    mongo_cursor_init( &cursor, &conn, collection );
    mongo_cursor_set_query( &cursor, &b );
    bool exists =  mongo_cursor_next( &cursor ) == MONGO_OK;
    bson_destroy( &b );
    mongo_cursor_destroy( &cursor );

    return exists;
}

bool test_password(const char *expected, const char *actual)
{
    char len_actual = strlen(actual);
    char len_expected = strlen(expected);

    D("len_actual: %d\n", len_actual)
    D("len_expected: %d\n", len_expected)
    D("expected:actual = %s:%s\n", expected, actual);

    for (int i = 0; i < len_actual; i++)     // vuln is here: strlen(actual)>127 => len_actual < 0 => no check [doesn't work]
    {
        if (i >= len_expected || expected[i] != actual[i])
            return false;
    }
    return true;
}

void user_create(char *user, char *pass)
{
    bson b;

    if (exists(COLLECTION_USERS, "user", user))
    {
        D("add_user: Error: already exists\n");
        WriteLn("Register: error (user already exists)");
        return;
    }

    bson_init( &b );
    bson_append_string( &b, "user", user);
    bson_append_string( &b, "pass", pass);
    bson_finish( &b );

    int result = mongo_insert( &conn, COLLECTION_USERS, &b, NULL );
    bson_destroy( &b );

    if ( result == MONGO_OK )
    {
        D("add_user: OK\n");
        WriteLn("Register: OK");
        user_login(user, pass);
    }
    else
    {
        D("add_user: Error: %d\n", conn.err);
        WriteLn("Register: error");
    }
}

void user_login(char *user, char *pass)
{
    userSet = false;

    bson b;
    mongo_cursor cursor;
    bson_iterator it;

    bson_init( &b );
    bson_append_string( &b, "user", user );
    bson_finish( &b );

    mongo_cursor_init( &cursor, &conn, COLLECTION_USERS );
    mongo_cursor_set_query( &cursor, &b );
    while ( mongo_cursor_next( &cursor ) == MONGO_OK )
    {
        if ( !bson_find( &it, mongo_cursor_bson( &cursor ), "pass" ))
            break;

        const char *origPass = bson_iterator_string( &it );
        if ( !test_password(origPass, pass) )            // vuln
            break;

        if ( !bson_find( &it, mongo_cursor_bson( &cursor ), "_id" ))
            break;

        bson_oid_to_string( bson_iterator_oid( &it ), currentUser );
        userSet = true;
    }
    bson_destroy( &b );
    mongo_cursor_destroy( &cursor );

    D("user_login(%s): %s\n", user, userSet ? "OK" : "error");
    WriteLn("Login %s", userSet ? "OK" : "error");
}

void say( char *message)
{
    if (!roomSet)
    {
        D("say: not in room\n");
        WriteLn("Please, log in first");
        return;
    }

    bson b;
    time_t tp = time(NULL);
    tp = mktime(localtime(&tp));

    bson_init( &b );
    bson_append_string( &b, "roomId", currentRoom);
    bson_append_string( &b, "userId", currentUser);
    bson_append_string( &b, "message", message);
    bson_append_int( &b, "time", tp);
    bson_finish( &b );

    if ( mongo_insert( &conn, COLLECTION_MESSAGES, &b, NULL ) != MONGO_OK )
    {
        D( "mongo_insert: %d\n", conn.err );
        WriteLn("Warning: cannot send your message");
    }

    bson_destroy( &b );
}

void room_create(char *name, char *pass)
{
    int result;
    bson b;

    if (!userSet)
    {
        D("room_create: denied: user not logged in\n");
        WriteLn("Error: not logged in");
        return;
    }

    if ( exists( COLLECTION_ROOMS, "room", name ) )
    {
        D("room_create: denied: room already exists\n");
        WriteLn("Error: room already exists");
        return;
    }

    bson_init( &b );
    bson_append_string( &b, "owner", currentUser );
    bson_append_string( &b, "room", name );
    if (pass != NULL)
    {
        bson_append_string( &b, "pass", pass );
        bson_append_bool( &b, "protected", 1 );
    }
    else
    {
        bson_append_bool( &b, "protected", 0 );
    }
    bson_finish( &b );
    result =  mongo_insert( &conn, COLLECTION_ROOMS, &b, NULL );
    bson_destroy( &b );

    if ( result == MONGO_OK )
    {
        D( "room_create: OK\n" );
        WriteLn("OK: created %s room", pass == NULL ? "public" : "private");
        room_join(name, pass);
    }
    else
    {
        D( "room_create: error: %d\n", conn.err );
        WriteLn("Error");
    }
}

void room_join(char *name, char *pass)
{
    if ( !exists( COLLECTION_ROOMS, "room", name ) )
    {
        D("room_join(%s): failed: no such room\n");
        WriteLn("Error: no such room");
        return;
    }

    roomSet = false;

    bson b;
    mongo_cursor cursor;
    bson_iterator it;

    bson_init( &b );
    bson_append_string( &b, "room", name );
    bson_append_bool( &b, "protected", pass == NULL ? 0 : 1 );
    bson_finish( &b );

    mongo_cursor_init( &cursor, &conn, COLLECTION_ROOMS );
    mongo_cursor_set_query( &cursor, &b );
    while ( mongo_cursor_next( &cursor ) == MONGO_OK )
    {
        if ( pass != NULL )
        {
            if ( !bson_find( &it, mongo_cursor_bson( &cursor ), "pass" ) )
                break;

            const char *origPass = bson_iterator_string( &it );
            if ( !test_password(origPass, pass) )
            {
                WriteLn("Error: invalid password");
                break;
            }
        }

        if ( !bson_find( &it, mongo_cursor_bson( &cursor ), "_id" ))
            break;

        bson_oid_to_string( bson_iterator_oid( &it ), currentRoom );
        roomSet = true;
    }
    bson_destroy( &b );
    mongo_cursor_destroy( &cursor );

    D("room_join(%s): %s\n", name, roomSet ? "OK" : "error");
    if (roomSet)
    {
        WriteLn("Welcome to %s room '%s' !", pass == NULL ? "public" : "private", name);
    }
    else
    {
        WriteLn("Error: cannot join room");
    }

    room_history();
}

int list()
{
    bson query[1];
    mongo_cursor cursor[1];
    bson_init( query );
    bson_finish( query );

    mongo_cursor_init( cursor, &conn, COLLECTION_ROOMS );
    mongo_cursor_set_query( cursor, query );

    while ( mongo_cursor_next( cursor ) == MONGO_OK )
    {
        bson_iterator iterator[1];
        if ( bson_find( iterator, mongo_cursor_bson( cursor ), "room" ))
        {
            WriteLn(bson_iterator_string( iterator) ) ;
        }
    }
    bson_destroy( query );
    mongo_cursor_destroy( cursor );
    return 0;
}

void room_history()
{
    char stime[32];

    bson query[1];
    mongo_cursor cursor[1];
    bson_init( query );
    bson_append_string( query, "roomId", currentRoom );
    bson_finish( query );

    mongo_cursor_init( cursor, &conn, COLLECTION_MESSAGES );
    mongo_cursor_set_query( cursor, query );

    while ( mongo_cursor_next( cursor ) == MONGO_OK )
    {
        bson_iterator iterator[1];
        if ( bson_find( iterator, mongo_cursor_bson( cursor ), "time" ))
        {
            time_t t = bson_iterator_int(iterator);
            strftime(stime, 32, "%F %H:%M:%S", localtime(&t));
            Write("[ %s ] ", stime);
        }

        if ( bson_find( iterator, mongo_cursor_bson( cursor ), "userId" ))
        {
            Write("%s: ", user_name(bson_iterator_string( iterator)));
        }

        if ( bson_find( iterator, mongo_cursor_bson( cursor ), "message" ))
        {
            WriteLn(bson_iterator_string(iterator) ) ;
        }
    }
}

const char *user_name(const char *userId)
{
    bson query[1];
    mongo_cursor cursor[1];
    bson_oid_t oid;
    bson_init( query );
    bson_oid_from_string( &oid, userId );
    bson_append_oid( query, "_id", &oid );
    bson_finish( query );

    mongo_cursor_init( cursor, &conn, COLLECTION_USERS );
    mongo_cursor_set_query( cursor, query );

    while ( mongo_cursor_next( cursor ) == MONGO_OK )
    {
        bson_iterator iterator[1];
        if ( bson_find( iterator, mongo_cursor_bson( cursor ), "user" ))
        {
            bson_destroy( query );
            mongo_cursor_destroy( cursor );
            return bson_iterator_string(iterator);
        }
    }
    bson_destroy( query );
    mongo_cursor_destroy( cursor );
    return " ";
}
