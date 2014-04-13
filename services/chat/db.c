#include "db.h"
#include "mongo.h"
#include "io.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "debug.h"
mongo conn;

int connect_db() {
  
  int status = mongo_client( &conn, "127.0.0.1", 27017 );

  if( status != MONGO_OK ) {
      switch ( conn.err ) {
        case MONGO_CONN_NO_SOCKET:  printf( "no socket\n" ); return 1;
        case MONGO_CONN_FAIL:       printf( "connection failed\n" ); return 1;
        case MONGO_CONN_NOT_MASTER: printf( "not master\n" ); return 1;
      }
  }


  return 0;
}

int destrot_db()
{
mongo_destroy(&conn);
return 0;
}

int add_user(char *user, char *pass) {
   bson b;
    int result;
    bson_init( &b );
    bson_append_string( &b, "user", user);
    bson_append_string( &b, "pass",pass);
    bson_finish( &b );
    if( mongo_insert( &conn, "chat.users", &b, NULL ) != MONGO_OK ) {
      D( "FAIL: Failed to insert document with error %d\n", conn.err );
      exit( 1 );
    }
    bson_destroy( &b );
    return 0;
}

int user_login(char *user, char *pass) {
	bson query[1];
	mongo_cursor cursor[1];
	bson_init( query );
	bson_append_string( query, "user", user );
	bson_finish( query );
	mongo_cursor_init( cursor, &conn, "chat.users" );
	mongo_cursor_set_query( cursor, query );
	while( mongo_cursor_next( cursor ) == MONGO_OK ) {
		bson_iterator iterator[1];
		if ( bson_find( iterator, mongo_cursor_bson( cursor ), "pass" )) {
			const char *origPass=bson_iterator_string( iterator ) ;
			if ( bson_find( iterator, mongo_cursor_bson( cursor ), "_id" )) {
			char oidhex[25];
			bson_oid_to_string( bson_iterator_oid( iterator ), oidhex );
            		D( "%s" , oidhex );
			}
		}
	}
	bson_destroy( query );
	mongo_cursor_destroy( cursor );
	return 0;
}

int say(int roomId, int userId, char *message)
{
    bson b;
    int result;
    bson_init( &b );
    bson_append_int( &b, "roomId", roomId);
    bson_append_int( &b, "userId", userId);
    bson_append_string( &b, "message",message);
    /* Finish the BSON obj. */
    bson_finish( &b );
    /* Insert the sample BSON document. */
    if( mongo_insert( &conn, "chat.message", &b, NULL ) != MONGO_OK ) {
      D( "FAIL: Failed to insert document with error %d\n", conn.err );
      exit( 1 );
    }

    bson_destroy( &b );
    return 0;
}

int room_create(char *name, int ownerId, char *pass)
{
    D("room_create: %s, %d, %s", name, ownerId, pass);

    bson b;
    int result;
    bson_init( &b );
    bson_append_string( &b, "room", name);
    bson_append_string( &b, "pass",pass);
    /* Finish the BSON obj. */
    bson_finish( &b );


    /* Insert the sample BSON document. */
    if( mongo_insert( &conn, "chat.rooms", &b, NULL ) != MONGO_OK ) {
      D( "FAIL: Failed to insert document with error %d\n", conn.err );
      exit( 1 );
    }

    bson_destroy( &b );

	return 0;
}

int list() {
	bson query[1];
	mongo_cursor cursor[1];
	bson_init( query );
	bson_finish( query );

	mongo_cursor_init( cursor, &conn, "chat.rooms" );
	mongo_cursor_set_query( cursor, query );

	while( mongo_cursor_next( cursor ) == MONGO_OK ) {
		bson_iterator iterator[1];
		if ( bson_find( iterator, mongo_cursor_bson( cursor ), "user" )) {
			WriteLn(bson_iterator_string( iterator) ) ;
		}
	}
	bson_destroy( query );
	mongo_cursor_destroy( cursor );
	return 0;
}
