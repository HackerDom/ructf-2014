#include <cstdio>

#include "ClientConnection.h"
#include "Packet.h"
#include "Chunk.h"
#include "logging.h"

Chunk * ClientConnection::readChunk(int size)
{
	char buffer[CHUNK_SIZE];
	size_t totalBytesRead = 0;
	while (totalBytesRead < size)
	{
		int bytesRead = read(sockfd, buffer + totalBytesRead, size - totalBytesRead);
		if (bytesRead == 0)
			return nullptr;
		totalBytesRead += bytesRead;
	}
	return new Chunk(buffer, size);
}
bool ClientConnection::writeChunk(const Chunk *chunk)
{
	if (chunk == nullptr)
		return true;
	int bytesWritten = write(sockfd, chunk->data, chunk->size);
	return bytesWritten == chunk->size;
}
ClientConnection::ClientConnection(int sockfd)
{
	this->sockfd = sockfd;
}
Packet * ClientConnection::receive()
{
	debug("Receiving request");
	Packet *packet = new Packet();
	int bytesRead = read(sockfd, &packet->header, sizeof(packet->header));
	if (bytesRead < sizeof(packet->header) || packet->header.direction != Direction::REQUEST)
	{
		debug("Discarding bad packet");
		delete packet;
		return nullptr;
	}
	if (packet->header.dataSize != 0)
	{
		packet->chunk = readChunk(packet->header.dataSize);
		if (packet->chunk == nullptr)
		{
			delete packet;
			return nullptr;
		}
	}
	else
		packet->chunk = nullptr;
	return packet;
}
bool ClientConnection::send(ResponseType type, const FileId *fileId, Chunk *data)
{
	Packet packet;
	memset(&packet.header, sizeof(packet.header), 0);
	packet.header.direction = Direction::RESPONSE;
	packet.header.type = type;
	if (fileId != nullptr)
		packet.header.fileId = *fileId;
	if (data != nullptr)
	{
		packet.header.dataSize = data->size;
	}
	int bytesWritten = write(sockfd, &packet.header, sizeof(packet.header));
	if (bytesWritten != sizeof(packet.header))
		return false;
	return writeChunk(data);
}
ClientConnection::~ClientConnection()
{
	close(sockfd);
}