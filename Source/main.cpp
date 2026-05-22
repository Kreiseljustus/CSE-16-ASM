#include <iostream>
#include <fstream>
#include <string>
#include "Assembler.h"
#include "ObjectFile.h"

#include "Linker.h"
#include <vector>

int main(int argc, char* argv[]) {
    std::string inputFile, outputFile;
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-I" && i + 1 < argc) inputFile = argv[++i];
        else if (arg == "-O" && i + 1 < argc) outputFile = argv[++i];
    }
    if (inputFile.empty())  inputFile = "program.asm";
    if (outputFile.empty()) outputFile = "program.bin";

    Assembler a = Assembler();
    std::vector<std::string> oFiles = a.assemble(inputFile);

    Linker linker;
    for (std::string s : oFiles) {
        linker.addObjectFile(s);
    }

    linker.link(outputFile);

    return 0;
}