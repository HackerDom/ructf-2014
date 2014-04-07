#pragma once

#include "Packet.h"

class ClientConnection
{
private:
	int sockfd;
	Chunk * readChunk(int size);
	bool writeChunk(const Chunk *chunk);
public:
	ClientConnection(int sockfd);
	Packet * receive();
	bool send(ResponseType type)
	{
		send(type, nullptr, nullptr);
	}
	bool send(ResponseType type, const FileId *fileId)
	{
		send(type, fileId, nullptr);
	}
	bool send(ResponseType type, const FileId *fileId, Chunk *data);
	~ClientConnection();
};
