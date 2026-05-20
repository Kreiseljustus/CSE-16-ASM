#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <string>
#include <cstdint>
#include <stdexcept>

#include "ObjectFile.h"

std::unordered_map<std::string, uint8_t> opcodes = {
    {"LD",    0x01},
    {"ADD",   0x02},
    {"SUB",   0x03},
    {"MUL",   0x04},
    {"DIV",   0x05},
    {"JEZ",   0x06},
    {"JNZ",   0x07},
    {"INP",   0x08},
    {"OUT",   0x09},
    {"STORE", 0x0A},
    {"LDMEM", 0x0B},
    {"JMP", 0x0C},
    {"LDPTR", 0x0D},
    {"STPTR", 0x0E },
    {"MOV", 0x0F},
    {"PUSH", 0x10},
    {"POP", 0x11},
    {"INC", 0x12},
    {"DEC", 0x13},
    {"SHL", 0x14},
    {"SHR", 0x15},
    {"NOT", 0x16},
    {"XOR", 0x17},
    {"OR", 0x18}, 
    {"AND", 0x19},
    {"CMP", 0x1A},
    {"JLT", 0x1B},
    {"JGT", 0x1C},
    {"CALL", 0x1D},
    {"RET", 0x1E},
    {"HLT",   0xFF}
};

std::unordered_map<std::string, uint16_t> registers = {
    {"A", 0}, {"B", 1}, {"C", 2}, {"D", 3}, {"SP", 4}, {"BP", 5 }
};

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

    std::ifstream file(inputFile);
    if (!file.is_open()) {
        std::cerr << "Could not open input file: " << inputFile << std::endl;
        return -1;
    }

    ObjectFile outputOBJ = ObjectFile();
    std::unordered_set<std::string> globalLabelNames;

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        size_t commentPos = line.find(';');
        if (commentPos != std::string::npos) line = line.substr(0, commentPos);
        lines.push_back(line);
    }

    //Get label addresses
    
    std::unordered_map<std::string, uint16_t> labels;
    uint16_t pc = 0;
    int lineIndex = 0;
    for (auto& l : lines) {
        lineIndex++;
        std::stringstream ss(l);
        std::string word;
        ss >> word;
        if (word.empty()) continue;
        if (word == "global") {
            std::string globalLabelName;
            if (!(ss >> globalLabelName)) {
                std::cout << "ERROR: Missing label name after 'global' keyword on line " << lineIndex - 1 << std::endl;
                continue;
            }
            //Resolves to symbol in second pass
            globalLabelNames.insert(globalLabelName);
            continue;
        }
        
		if (word.back() == ':') {
			labels[word.substr(0, word.size() - 1)] = pc;
		}
		else {
			if (opcodes.find(word) != opcodes.end())
				pc += 5;
		}
    }

    // helper to emit a 16-bit little-endian value
    std::vector<uint8_t> program;
    auto emit16 = [&](uint16_t value) {
        program.push_back(value & 0xFF);
        program.push_back((value >> 8) & 0xFF);
        };

    pc = 0;

    //produce bytecode
    for (auto& l : lines) {
        std::stringstream ss(l);
        std::string word;
        ss >> word;
        if (word.empty()) continue;
        if (word == "global") continue;
        if (word.back() == ':') {
            std::string labelName = word.substr(0, word.size() - 1);

            bool global = false;

            if (globalLabelNames.contains(labelName)) {
                global = true;
            }
            
            Symbol s = Symbol(labelName, pc, global);
            outputOBJ.addSymbol(s);

            continue;
        }

        auto it = opcodes.find(word);
        if (it == opcodes.end()) {
            std::cerr << "Unknown opcode: " << word << std::endl;
            continue;
        }

        program.push_back(it->second);

        // collect up to 2 arguments
        std::vector<uint16_t> args;
        std::string arg;
        while (ss >> arg && args.size() < 2) {
            uint16_t value;
            if (registers.find(arg) != registers.end())
                value = registers[arg];
            else if (labels.find(arg) != labels.end())
                value = labels[arg];
            else
				try {
                    //immidiate value
					value = static_cast<uint16_t>(std::stoi(arg, nullptr, 0));
				}
			    catch (std::invalid_argument&) {
				    //Unknown label cause not a number
				    value = 0x0000;

                    uint8_t offset;
                    if (args.size() == 0) offset = pc + 1;
                    if (args.size() == 1) offset = pc + 3;

                    Relocation r = Relocation{ offset, arg};
                    outputOBJ.addRelocation(r);
			    }

            std::cout << "arg: " << arg << " = 0x" << std::hex << value << std::endl;
            args.push_back(value);
        }

        emit16(args.size() > 0 ? args[0] : 0);
        emit16(args.size() > 1 ? args[1] : 0);

        pc += 5;
    }

    if (program.size() <= 2) {
        std::cerr << "No instructions assembled!" << std::endl;
        return -1;
    }

    std::cout << "Assembled " << (program.size() - 2) << " bytes." << std::endl;

    outputOBJ.setCode(program);
    outputOBJ.writeFile("test.o", true);

    return 0;
}