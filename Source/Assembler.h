#pragma once

#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

class Assembler {
public:
	std::vector<std::string> assemble(std::string_view path);
private:
	std::unordered_set<std::string> assembledFiles;
};