#include <string_view>

#include "Linker.h"
#include "ObjectFile.h"
#include <cstdint>
#include <string>
#include <iostream>
#include <fstream>

void Linker::link(std::string_view path) {
	calculateBaseAddresses();
	resolveRelocations();
	writeOutput(path);
}

void Linker::addObjectFile(std::string_view path) {
	ObjectFile o = ObjectFile();
	o.parseFile(path, nullptr);

	objectFiles.push_back(o);
}

void Linker::calculateBaseAddresses() {
	for (int i = 0; i < objectFiles.size(); i++) {
		ObjectFile& o = objectFiles.at(i);

		uint16_t additionalOffset = 0;

		//Loop through each object file before this one
		for (int j = 0; j < i; j++) {
			ObjectFile& before = objectFiles.at(j);
			additionalOffset += before.getCode().size();
		}

		for (Symbol& s : o.getSymbols()) {
			//5 Bytes for jmp to main instruction at the start of the finished executable
			s.offset += additionalOffset;
		}
	}
}

void Linker::resolveRelocations() {
	for (ObjectFile& o : objectFiles) {
		for (Relocation r : o.getRelocations()) {
			std::string name = r.symbol;
			uint16_t codeOffset = r.offset;
			bool found = false;
			for (ObjectFile& other : objectFiles) {
				for (Symbol s : other.getSymbols()) {
					if (s.name == name) {
						o.getCode()[codeOffset] = (s.offset + 5) & 0xFF;
						o.getCode()[codeOffset + 1] = ((s.offset + 5) >> 8) & 0xFF;
						found = true;
					}
				}
			}
			if (!found) {
				std::cout << "Linker error: unresolved external symbol: " << name << std::endl;
			}
		}
	}
}

void Linker::writeOutput(std::string_view path) {
	std::string spath(path);

	if (!spath.ends_with(".bin")) spath += ".bin";

	std::ofstream file(spath, std::ios::binary);

	if (!file.is_open()) {
		std::cout << "Failed to open output file!" << std::endl;
	}

	uint8_t magicF = 0xFE;
	uint8_t magic1 = 0x10;
	file.write(reinterpret_cast<const char*>(&magicF), 1);
	file.write(reinterpret_cast<const char*>(&magic1), 1);

	//Jump opcode
	uint8_t jmp = 0x0C;
	file.write(reinterpret_cast<const char*>(&jmp), 1);

	bool foundMain = false;
	for (ObjectFile& o : objectFiles) {
		for (Symbol& s : o.getSymbols()) {
			if (s.name == "main" && s.global) {
				//5 = 1 byte jmp + 2 bytes address + 2 bytes padding
				uint16_t finalOffset = s.offset + 5;
				file.write(reinterpret_cast<const char*>(&finalOffset), 2);
				foundMain = true;
			}
		}
	}

	if (!foundMain) {
		std::cout << "Linker error: Unable to find a global label called 'main'" << std::endl;
		return;
	}

	uint8_t zero = 0x00;
	file.write(reinterpret_cast<const char*>(&zero), 1);
	file.write(reinterpret_cast<const char*>(&zero), 1);

	for (ObjectFile& o : objectFiles) {
		file.write(reinterpret_cast<const char*>(o.getCode().data()), o.getCode().size());
	}

	file.close();
}
