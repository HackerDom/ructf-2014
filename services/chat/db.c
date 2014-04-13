#include "db.h"
#include "mongo.h"
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
    bson_append_new_oid( &b, "user_id" );

    bson_append_string( &b, "user", user);
    bson_append_string( &b, "pass",pass);
    /* Finish the BSON obj. */
    bson_finish( &b );


    /* Insert the sample BSON document. */
    if( mongo_insert( &conn, "chat.users", &b, NULL ) != MONGO_OK ) {
      D( "FAIL: Failed to insert document with error %d\n", conn.err );
      exit( 1 );
    }

    bson_destroy( &b );
    return 0;
}

int user_login(char *user, char *pass) {
	return 0;
}

int room_create(char *name, int ownerId, char *pass)
{
    D("room_create: %s, %d, %s", name, ownerId, pass);

    bson b;
    int result;
    bson_init( &b );
    bson_append_new_oid( &b, "room_id" );

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
