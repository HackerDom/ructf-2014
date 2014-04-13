#include "db.h"
#include "mongo.h"
#include "io.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "debug.h"

#define MONGO_HOST "127.0.0.1"
#define MONGO_PORT 27017

mongo conn;

int connect_db()
{
    int status = mongo_client( &conn, MONGO_HOST, MONGO_PORT );
    return status == MONGO_OK ? 0 : -1;
}

void disconnect_db()
{
    mongo_destroy(&conn);
}

int add_user(char *user, char *pass)
{
    bson b;
    int result;
    bson_init( &b );
    bson_append_string( &b, "user", user);
    bson_append_string( &b, "pass", pass);
    bson_finish( &b );
    if ( mongo_insert( &conn, "chat.users", &b, NULL ) != MONGO_OK )
    {
        D( "FAIL: Failed to insert document with error %d\n", conn.err );
        exit( 1 );
    }
    bson_destroy( &b );
    return 0;
}

int user_login(char *user, char *pass)
{
    bson query[1];
    mongo_cursor cursor[1];
    bson_init( query );
    bson_append_string( query, "user", user );
    bson_finish( query );
    mongo_cursor_init( cursor, &conn, "chat.users" );
    mongo_cursor_set_query( cursor, query );
    while ( mongo_cursor_next( cursor ) == MONGO_OK )
    {
        bson_iterator iterator[1];
        if ( bson_find( iterator, mongo_cursor_bson( cursor ), "pass" ))
        {
            const char *origPass = bson_iterator_string( iterator ) ;
            if ( bson_find( iterator, mongo_cursor_bson( cursor ), "_id" ))
            {
                char oidhex[25];
                bson_oid_to_string( bson_iterator_oid( iterator ), currentUser  );
            }
        }
    }
    bson_destroy( query );
    mongo_cursor_destroy( cursor );
    return 0;
}

int say( char *message)
{
    D("asdsad");
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
    if ( mongo_insert( &conn, "chat.msg", &b, NULL ) != MONGO_OK )
    {
        D( "FAIL: Failed to insert document with error %d\n", conn.err );
        exit( 1 );
    }

    bson_destroy( &b );
    return 0;
}

int room_create(char *name, char *ownerId, char *pass)
{
    bson b;
    int result;
    bson_init( &b );
    bson_append_string(&b, "ownerId", ownerId);
    bson_append_string( &b, "room", name);
    bson_append_string( &b, "pass", pass);
    bson_finish( &b );
    if ( mongo_insert( &conn, "chat.rooms", &b, NULL ) != MONGO_OK )
    {
        D( "FAIL: Failed to insert document with error %d\n", conn.err );
        exit( 1 );
    }
    bson_destroy( &b );
    return 0;
}

int room_join(char *name, char *pass)
{

    bson query[1];
    mongo_cursor cursor[1];
    bson_init( query );
    bson_append_string( query, "room", name );
    bson_finish( query );
    mongo_cursor_init( cursor, &conn, "chat.rooms" );
    mongo_cursor_set_query( cursor, query );
    while ( mongo_cursor_next( cursor ) == MONGO_OK )
    {
        bson_iterator iterator[1];
        if ( bson_find( iterator, mongo_cursor_bson( cursor ), "pass" ))
        {
            const char *origPass = bson_iterator_string( iterator ) ;
            if ( bson_find( iterator, mongo_cursor_bson( cursor ), "_id" ))
            {
                bson_oid_to_string( bson_iterator_oid( iterator ), currentRoom);
                D(currentRoom);
            }
        }
    }
    bson_destroy( query );
    mongo_cursor_destroy( cursor );
    return 0;

}

int list()
{
    bson query[1];
    mongo_cursor cursor[1];
    bson_init( query );
    bson_finish( query );

    mongo_cursor_init( cursor, &conn, "chat.rooms" );
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

    mongo_cursor_init( cursor, &conn, "chat.msg" );
    mongo_cursor_set_query( cursor, query );

    while ( mongo_cursor_next( cursor ) == MONGO_OK )
    {
        bson_iterator iterator[1];
        if ( bson_find( iterator, mongo_cursor_bson( cursor ), "time" ))
        {
            WriteLn("%d", bson_iterator_time_t(iterator)) ;
        }
        if ( bson_find( iterator, mongo_cursor_bson( cursor ), "message" ))
        {
            WriteLn(bson_iterator_string( iterator) ) ;
        }
    }
    bson_oid_t oid;
    bson_destroy( query );
    mongo_cursor_destroy( cursor );
    bson_init( query );
    bson_oid_from_string( &oid, currentUser );
    bson_append_oid( query, "_id", &oid );
    bson_finish( query );

    mongo_cursor_init( cursor, &conn, "chat.users" );
    mongo_cursor_set_query( cursor, query );

    while ( mongo_cursor_next( cursor ) == MONGO_OK )
    {
        bson_iterator iterator[1];
        if ( bson_find( iterator, mongo_cursor_bson( cursor ), "user" ))
        {
            Write("%s  ", bson_iterator_string(iterator)) ;
        }
    }
    bson_destroy( query );
    mongo_cursor_destroy( cursor );
    return 0;
}
