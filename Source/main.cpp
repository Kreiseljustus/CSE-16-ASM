#include <iostream>
#include <fstream>
#include <string>
#include "Assembler.h"
#include "ObjectFile.h"

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

    ObjectFile test = ObjectFile();
    bool bit;
    test.parseFile("test.o", &bit);

    return 0;
}