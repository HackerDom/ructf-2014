#pragma once

#include <fstream>

#include "Chunk.h"

struct FileId
{
	char data[16];
	FileId()
	{
	}
	FileId(unsigned char *data)
	{
		memcpy(this->data, data, 16);
	}
	operator const unsigned char *()
	{
		return (const unsigned char *)data;
	}
};

class File
{
private:
	static const int MaxSize = 2 * 1024 * 1024;
	std::fstream *stream;
	bool ok;
	bool uncommitted;
	std::string name;
public:
	FileId fileId;
	File(std::fstream *stream, const FileId fileId, const std::string &name, bool isNew);
	Chunk *readChunk();
	bool writeChunk(const Chunk *chunk);
	bool isEof();
	bool commit();
	~File();
};