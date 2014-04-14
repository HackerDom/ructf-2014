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

int user_create(char *user, char *pass)
{
    bson b;

    if (exists(COLLECTION_USERS, "user", user))
    {
        D("add_user: Error: already exists\n");
        return -1;
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
        return user_login(user, pass);
    }
    else
    {
        D( "add_user: Error: %d\n", conn.err );
        return -1;
    }
}

int user_login(char *user, char *pass)
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
    return userSet ? 0 : -1;
}

int say( char *message)
{
    if (!roomSet)
    {
        D("say: not in room\n");
        return -1;
    }

    D("say\n");

    bson b;
    int result;
    time_t tp;
    tp = time(NULL);
    D("%d", tp);
    bson_init( &b );
    bson_append_string( &b, "roomId", currentRoom);
    bson_append_string( &b, "userId", currentUser);
    bson_append_string( &b, "message", message);
    bson_append_time_t(&b, "time", tp);
    /* Finish the BSON obj. */
    bson_finish( &b );
    /* Insert the sample BSON document. */
    if ( mongo_insert( &conn, COLLECTION_MESSAGES, &b, NULL ) != MONGO_OK )
    {
        D( "FAIL: Failed to insert document with error %d\n", conn.err );
        exit( 1 );
    }

    bson_destroy( &b );
    return 0;
}

int room_create(char *name, char *pass)
{
    int result;
    bson b;

    if (!userSet)
    {
        D("room_create: denied: user not logged in\n");
        return -1;
    }

    if ( exists( COLLECTION_ROOMS, "room", name ) )
    {
        D("room_create: denied: room already exists\n");
        return -1;
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
        return room_join(name, pass);
    }
    else
    {
        D( "room_create: error: %d\n", conn.err );
        return -1;
    }
}

int room_join(char *name, char *pass)
{
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
                break;
        }

        if ( !bson_find( &it, mongo_cursor_bson( &cursor ), "_id" ))
            break;

        bson_oid_to_string( bson_iterator_oid( &it ), currentRoom );
        roomSet = true;
    }
    bson_destroy( &b );
    mongo_cursor_destroy( &cursor );

    D("room_join(%s): %s\n", name, roomSet ? "OK" : "error");
    return roomSet ? 0 : -1;
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

int list_room()
{
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
        if ( bson_find( iterator, mongo_cursor_bson( cursor ), "userId" ))
        {
           Write("%s - ", user_name(bson_iterator_string( iterator)));
        }
        if ( bson_find( iterator, mongo_cursor_bson( cursor ), "time" ))
        {
            WriteLn("%d:", bson_iterator_time_t(iterator)) ;
        }
        if ( bson_find( iterator, mongo_cursor_bson( cursor ), "message" ))
        {
            WriteLn(bson_iterator_string( iterator) ) ;
        }
    }

    return 0;
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
