#include "db.h"
#include "mongo.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "debug.h"


int connect_db() {
  mongo conn[1];
  int status = mongo_client( conn, "127.0.0.1", 27017 );

  if( status != MONGO_OK ) {
      switch ( conn->err ) {
        case MONGO_CONN_NO_SOCKET:  printf( "no socket\n" ); return 1;
        case MONGO_CONN_FAIL:       printf( "connection failed\n" ); return 1;
        case MONGO_CONN_NOT_MASTER: printf( "not master\n" ); return 1;
      }
  }

  mongo_destroy( conn );

  return 0;
}