#pragma once

#include <vector>

#include "ObjectFile.h"
#include <string_view>

class Linker {
public:
	void link(std::string_view path);
private:
	void addObjectFile(std::string_view path);
private:
	std::vector<ObjectFile> objectFiles;
};