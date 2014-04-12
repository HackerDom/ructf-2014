#include "server.h"
#include "store.h"

static const int port = 4242;
static const char *upload_dir = "uploads";

int main() {
	Store store;

	mb_store_init(&store, upload_dir);
	mb_start_server(port, &store);
}