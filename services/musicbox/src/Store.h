#pragma once

#include <string>

#include "File.h"

class Store
{
private:
	std::string dir;
	FileId generateFileId();
	std::string fileIdToString(FileId fileId);
public:
	Store(const std::string &dir);
	File *queryFile(const FileId fileId);
	File *createFile();
};