#include "ObjectFile.h"

#include <fstream>
#include <cstdint>
#include <string_view>

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
        file.write(reinterpret_cast<const char*>(&symbol.offset), 2);
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
    
    return true;
}
