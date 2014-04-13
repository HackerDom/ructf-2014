#include <string>
#include <fstream>
#include <uuid/uuid.h>

#include "Store.h"
#include "File.h"
#include "logging.h"

FileId Store::generateFileId()
{
	uuid_t uuid;
	uuid_generate_random (uuid);
	return (FileId)uuid;
}
std::string Store::fileIdToString(FileId fileId)
{
	char s[37];
	uuid_unparse(fileId, s);
	return s;
}
Store::Store(const std::string &dir)
{
	this->dir = dir;
}
File *Store::queryFile(const FileId fileId)
{
	auto name = dir + "/" + fileIdToString(fileId);
	auto stream = new std::fstream(name, std::fstream::in);
	if (stream->good())
		return new File(stream, fileId, name, false);
	delete stream;
	return nullptr;
}
File *Store::createFile()
{
	auto id = generateFileId();
	auto name = dir + "/" + fileIdToString(id);
	debug("Creating file %s", name.c_str());
	auto stream = new std::fstream(name, std::fstream::out);
	if (stream->good())
		return new File(stream, id, name, true);
	delete stream;
	return nullptr;
}