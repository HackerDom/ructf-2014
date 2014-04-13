
#include <cstdio>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#include "Server.h"
#include "Chunk.h"
#include "logging.h"

void Server::processClient(int sockfd)
{
	debug("Processing new client");

	ClientConnection conn(sockfd);
	auto request = conn.receive();
	if (request == nullptr)
		return;

	debug("Parsed packet");

	switch (request->header.type)
	{
	case RequestType::GET:
		if (!processGet(request, conn))
			conn.send(ResponseType::ERROR);
		break;
	case RequestType::PUT:
		if (!processPut(request, conn))
			conn.send(ResponseType::ERROR);
		break;
	default:
		conn.send(ResponseType::ERROR);
	}
	delete request;
}
bool Server::processGet(const Packet *request, ClientConnection &conn)
{
	debug("Processing GET");
	auto file = store->queryFile(request->header.fileId);
	debug("Opened file");
	if (file == nullptr)
		return false;
	bool success = true;
	Chunk *chunk;
	while (success && ((chunk = file->readChunk()) != nullptr))
	{
		debug("Sending chunk of size %d", chunk->size);
		success &= conn.send(ResponseType::SUCCESS, &file->fileId, chunk);
		delete chunk;
	}
	success &= file->isEof();
	debug("Finished sending file (%d)", success);
	delete file;
	return success;
}
bool Server::processPut(const Packet *request, ClientConnection &conn)
{
	debug("Processing PUT");
	auto file = store->createFile();
	if (file == nullptr)
		return false;
	debug("Created new file");
	bool success = true;
	Packet *packet = (Packet *)request;
	do
	{
		//TODO: validate meta
		success &= file->writeChunk(packet->chunk);
		debug("Successfully wrote chunk");
		if (packet != request)
			delete packet;
		debug("Deleted packet");
	}
	while (success && ((packet = conn.receive()) != nullptr));
	debug("Sending confirmation (%d)", success);
	if (success && (success &= file->commit()))
		conn.send(ResponseType::SUCCESS, &file->fileId);
	delete file;
	return success;
}
Server::Server(Store &store)
{
	this->store = &store;
}
void Server::listen(int port)
{
	int listener = socket(AF_INET, SOCK_STREAM, 0);
	if (listener < 0) 
		error("Error opening listener socket");
	int yes = 1;
	if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		error("Error on setsockopt");

	//TODO: setsockopt timeout

	sockaddr_in servAddr;
	bzero((char *)&servAddr, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = INADDR_ANY;
	servAddr.sin_port = htons(port);

	if (bind(listener, (sockaddr *)&servAddr, sizeof(servAddr)) < 0)
		error("Error on binding");

	::listen(listener, 5);

	sockaddr_in clientAddr;
	size_t clilen = sizeof(clientAddr);
	while (1) 
	{
		int newsockfd = accept(listener, (sockaddr *)&clientAddr, &clilen);
		if (newsockfd < 0)
			error("Error on accept");

		int pid = fork();
		if (pid < 0)
			error("Error on fork");
		if (pid == 0)  
		{
			close(listener);
			processClient(newsockfd);
			exit(0);
		}
		else
		{
			close(newsockfd);
		}
	}
}