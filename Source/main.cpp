#include <iostream>
#include <fstream>
#include <string>
#include "Assembler.h"
#include "ObjectFile.h"

#include "Linker.h"

int main(int argc, char* argv[])
{
    std::string inputFile, outputFile;
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-I" && i + 1 < argc) inputFile = argv[++i];
        else if (arg == "-O" && i + 1 < argc) outputFile = argv[++i];
    }
    if (inputFile.empty())  inputFile = "program.asm";
    if (outputFile.empty()) outputFile = "program.bin";

    Assembler a = Assembler();
    a.assemble(inputFile);

    return 0;
}