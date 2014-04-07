#pragma once

#include <cstring>

#include "Chunk.h"
#include "File.h"

enum Direction : char
{
	REQUEST,
	RESPONSE
};

enum RequestType : char
{
	GET,
	PUT
};

enum ResponseType : char
{
	ERROR,
	SUCCESS
};

struct Packet
{
public:
#pragma pack(1)
	struct
	{
		char direction;
		char type;
		short dataSize;
		FileId fileId;
	} header;
	Chunk *chunk;
	Packet()
	{
		memset(&header, sizeof(header), 0);
		chunk = nullptr;
	}
	~Packet()
	{
		if (chunk != nullptr)
			delete chunk;
	}
};