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

#define COLLECTION_USERS "chat.users"

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

bool user_exists(char *user)
{
    bson b;
    mongo_cursor cursor;
    bson_iterator it;

    bson_init( &b );
    bson_append_string( &b, "user", user);
    bson_finish( &b );

    mongo_cursor_init( &cursor, &conn, COLLECTION_USERS );
    mongo_cursor_set_query( &cursor, &b );
    bool exists =  mongo_cursor_next( &cursor ) == MONGO_OK;
    bson_destroy( &b );
    mongo_cursor_destroy( &cursor );

    return exists;
}

int add_user(char *user, char *pass)
{
    bson b;

    if (user_exists(user))
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
        return 0;
    }
    else
    {
        D( "add_user: Error: %d\n", conn.err );
        return -1;
    }
}

int user_login(char *user, char *pass)
{
    bool found = false;
    bson b;
    mongo_cursor cursor;
    bson_iterator it;

    bson_init( &b );
    bson_append_string( &b, "user", user );
    bson_append_string( &b, "pass", pass );
    bson_finish( &b );

    mongo_cursor_init( &cursor, &conn, COLLECTION_USERS );
    mongo_cursor_set_query( &cursor, &b );
    while ( mongo_cursor_next( &cursor ) == MONGO_OK )
    {
        if ( bson_find( &it, mongo_cursor_bson( &cursor ), "_id" ))
        {
            bson_oid_to_string( bson_iterator_oid( &it ), currentUser );
            found = true;
        }
    }
    bson_destroy( &b );
    mongo_cursor_destroy( &cursor );

    return found ? 0 : -1;
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
		if ( bson_find( iterator, mongo_cursor_bson( cursor ), "userId" ))
        {
		user_name(bson_iterator_string( iterator));
			}
        if ( bson_find( iterator, mongo_cursor_bson( cursor ), "time" ))
        {
            WriteLn("%d", bson_iterator_time_t(iterator)) ;
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
    bson_destroy( query );
    mongo_cursor_destroy( cursor );
    bson_init( query );
    bson_oid_from_string( &oid, userId );
    bson_append_oid( query, "_id", &oid );
    bson_finish( query );

    mongo_cursor_init( cursor, &conn, "chat.users" );
    mongo_cursor_set_query( cursor, query );

    while ( mongo_cursor_next( cursor ) == MONGO_OK )
    {
        bson_iterator iterator[1];
        if ( bson_find( iterator, mongo_cursor_bson( cursor ), "user" ))
        {
		   bson_destroy( query );
		   mongo_cursor_destroy( cursor );
           return bson_iterator_string(iterator) ;
        }
    }
    bson_destroy( query );
    mongo_cursor_destroy( cursor );
	return " ";
}
