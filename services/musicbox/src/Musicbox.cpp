#include "Store.h"
#include "Server.h"

int main()
{
	Store store("uploads");
	Server server(store);
	server.listen(16701);
}