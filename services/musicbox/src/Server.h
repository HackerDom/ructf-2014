#pragma once

#include "Packet.h"
#include "ClientConnection.h"
#include "Store.h"

class Server
{
private:
	Store *store;
	void processClient(int sockfd);
	bool processGet(const Packet *request, ClientConnection &conn);
	bool processPut(const Packet *request, ClientConnection &conn);
public:
	Server(Store &store);
	void listen(int port);
};