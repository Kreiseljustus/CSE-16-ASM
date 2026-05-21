#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <string_view>

//Basically name of method, offset is from file start (0x00)

//What symbols this file offers
struct Symbol {
	std::string name;
	uint16_t offset;
	bool global = false;
};

//If symbol is defined but not found add relocation
//that will be used by linker to find the symbol and patch it with a real address later

//Symbols this file is missing / are unknown
struct Relocation {
	uint16_t offset;
	std::string symbol;
};

//Assembler -> Object File with info whats missing basically -> Linker figures out where the missing stuff is

class ObjectFile {
public:
	ObjectFile();

	void addSymbol(Symbol& symbol) { symbols.push_back(symbol); }
	void addRelocation(Relocation& relocation) { relocations.push_back(relocation); }
	void setCode(std::vector<uint8_t>& code) { this->code = code; }

	bool writeFile(std::string_view path, bool is16Bit);
	bool parseFile(std::string_view path, bool* is16Bit);
private:
	//Complete assembled code
	//Unknown symbol locations are 0x0000
	std::vector<uint8_t> code;
	std::vector<Symbol> symbols;
	std::vector<Relocation> relocations;
};