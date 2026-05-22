#include "ObjectFile.h"

#include <fstream>
#include <cstdint>
#include <string_view>

#include <iostream>
#include <string>
#include <string.h>

ObjectFile::ObjectFile() {
}

bool ObjectFile::writeFile(std::string_view path, bool is16Bit) {

    std::ofstream file(path.data(), std::ios::binary | std::ios::out);

    if (!file.is_open()) {
        return false;
    }

    file.write(is16Bit ? "CSE16" : "CSE08", 5);
    file.write("V1", 2);

    uint32_t codeSize = (uint32_t)code.size();
    file.write(reinterpret_cast<const char*>(&codeSize), sizeof(uint32_t));

    file.write(reinterpret_cast<const char*>(code.data()), codeSize);

    uint32_t symbolCount = (uint32_t)symbols.size();
    file.write(reinterpret_cast<const char*>(&symbolCount), 4);

    for (Symbol& symbol : symbols) {
        file.write(reinterpret_cast<const char*>(&symbol.offset), 4);
        file.write(reinterpret_cast<const char*>(&symbol.global), 1);

        uint8_t symbolNameLength = symbol.name.length();
        file.write(reinterpret_cast<const char*>(&symbolNameLength), 1);
        file.write(reinterpret_cast<const char*>(symbol.name.c_str()), symbolNameLength);
    }

    uint32_t relocationCount = (uint32_t)relocations.size();
    file.write(reinterpret_cast<const char*>(&relocationCount), 4);

    for (Relocation& relocation : relocations) {
        file.write(reinterpret_cast<const char*>(&relocation.offset), 4);
        uint8_t symbolNameLength = relocation.symbol.length();
        file.write(reinterpret_cast<const char*>(&symbolNameLength), 1);
        file.write(reinterpret_cast<const char*>(relocation.symbol.c_str()), relocation.symbol.length());
    }

    file.close();
    
    return true;
}

bool ObjectFile::parseFile(std::string_view path, bool* is16Bit) {
    if (!path.ends_with(".o")) {
        std::cout << "File is not an object file: " << path << std::endl;
        return false;
    }

    std::ifstream file(path.data(), std::ios::binary);

    if (!file.is_open()) {
        std::cout << "Failed to open file " << path << std::endl;
        return false;
    }
    
    char magic[5];

    file.read(magic, 5);
    if (std::memcmp(magic, "CSE08", 5) != 0 && memcmp(magic, "CSE16", 5) != 0) {
        std::cout << "Failed to parse file: " << path << " > Not a valid format" << std::endl;
        return false;
    }

    try {
        if (is16Bit != nullptr) {
            *is16Bit = true;
            if (std::memcmp(magic, "CSE08", 5) == 0) *is16Bit = false;
        }

        char version[2];
        file.read(version, 2);

        uint32_t code_size;
        file.read(reinterpret_cast<char*>(&code_size), sizeof(uint32_t));

        for (int i = 0; i < code_size; i++) {
            uint8_t byte;
            file.read(reinterpret_cast<char*>(&byte), 1);

            code.push_back(byte);
        }

        uint32_t symbolCount;
        file.read(reinterpret_cast<char*>(&symbolCount), 4);

        for (int i = 0; i < symbolCount; i++) {
            uint32_t offset;
            file.read(reinterpret_cast<char*>(&offset), 4);
            bool global;
            file.read(reinterpret_cast<char*>(&global), 1);
            char nameLength;
            file.read(reinterpret_cast<char*>(&nameLength), 1);
            char* name = new char[nameLength];
            file.read(name, nameLength);

            std::string sName(name, nameLength);

            Symbol s = Symbol(sName, (uint16_t)offset, global);

            std::cout << "symbol name " << sName << " with offset " << offset << " is global: " << global << std::endl;

            symbols.push_back(s);
        }

        uint32_t relocationCount;
        file.read(reinterpret_cast<char*>(&relocationCount), 4);

        for (int i = 0; i < relocationCount; i++) {
            uint32_t offset;
            file.read(reinterpret_cast<char*>(&offset), 4);
            char nameLength;
            file.read(reinterpret_cast<char*>(&nameLength), 1);
            char* name = new char[nameLength];
            file.read(name, nameLength);

            std::string sName(name, nameLength);

            Relocation r = Relocation(offset, sName);

            std::cout << "relocation name " << sName << " with offset " << offset << std::endl;;

            relocations.push_back(r);
        }

        file.close();

        return true;
    }
    catch (...) {
        return false;
    }
}
