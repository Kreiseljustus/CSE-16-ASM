#pragma once

#include <vector>

#include "ObjectFile.h"
#include <string_view>
#include <cstdint>

class Linker {
public:
	Linker() = default;

	void link(std::string_view path);
	void addObjectFile(std::string_view path);
private:
	//void addObjectFile(std::string_view path);
	void calculateBaseAddresses();
	void resolveRelocations();
	void writeOutput(std::string_view path);
private:
	std::vector<ObjectFile> objectFiles;
	//In order of the object files
	std::vector<uint16_t> offsets;
};