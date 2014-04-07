#include <cstdlib>
#include <fstream>

#include "File.h"
#include "logging.h"

File::File(std::fstream *stream, FileId fileId, const std::string &name, bool isNew)
{
	this->stream = stream;
	this->fileId = fileId;
	this->name = name;
	ok = true;
	uncommitted = isNew;
}
Chunk *File::readChunk()
{
	char buffer[CHUNK_SIZE];
	ok &= !isEof();
	if (!ok)
		return nullptr;
	stream->read(buffer, CHUNK_SIZE);
	int count = stream->gcount();
	ok &= count > 0 && stream->tellg() <= MaxSize;
	if (!ok)
		return nullptr;
	return new Chunk(buffer, count);
}
bool File::writeChunk(const Chunk *chunk)
{
	debug("Writing chunk of size %d", chunk->size);
	debug("ok = %d, checking %d + %d <= %d", ok, (int)stream->tellp(), chunk->size, MaxSize);
	ok &= uncommitted && ((int)stream->tellp() + chunk->size) <= MaxSize;
	debug("ok = %d", ok);
	if (ok)
		stream->write(chunk->data, chunk->size);
	debug("Writing chunk finished");
	return ok &= stream->good();
}
bool File::isEof()
{
	return stream->eof();
}
bool File::commit()
{
	uncommitted = false;
	return ok;
}
File::~File()
{
	stream->close();
	if (uncommitted)
	{
		remove(name.c_str());
	}
}
