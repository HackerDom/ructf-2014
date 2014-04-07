#pragma once

#include <cstring>

#define CHUNK_SIZE 0x8000

struct Chunk
{
	int size;
	char data[CHUNK_SIZE];
	Chunk(char *data, int size)
	{
		this->size = size;
		memcpy(this->data, data, size);
	}
};