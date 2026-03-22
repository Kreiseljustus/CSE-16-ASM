#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <string>

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
    {"HLT",   0xFF}
};

std::unordered_map<std::string, uint16_t> registers = {
    {"A", 0}, {"B", 1}, {"C", 2}, {"D", 3}
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

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        size_t commentPos = line.find(';');
        if (commentPos != std::string::npos) line = line.substr(0, commentPos);
        lines.push_back(line);
    }

    // first pass — collect labels
    // every instruction is exactly 5 bytes, labels resolve to byte offsets
    std::unordered_map<std::string, uint16_t> labels;
    uint16_t pc = 0;
    for (auto& l : lines) {
        std::stringstream ss(l);
        std::string word;
        ss >> word;
        if (word.empty()) continue;
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

    // magic header — 0xFE 0x10 identifies this as a CSE16 binary
    program.push_back(0xFE);
    program.push_back(0x10);

    // second pass — emit instructions
    for (auto& l : lines) {
        std::stringstream ss(l);
        std::string word;
        ss >> word;
        if (word.empty()) continue;
        if (word.back() == ':') continue;

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
                value = static_cast<uint16_t>(std::stoi(arg, nullptr, 0));

            std::cout << "arg: " << arg << " = 0x" << std::hex << value << std::endl;
            args.push_back(value);
        }

        // always emit exactly 2 args, pad with 0 if missing
        emit16(args.size() > 0 ? args[0] : 0);
        emit16(args.size() > 1 ? args[1] : 0);
    }

    if (program.size() <= 2) {
        std::cerr << "No instructions assembled!" << std::endl;
        return -1;
    }

    std::ofstream out(outputFile, std::ios::binary);
    out.write(reinterpret_cast<const char*>(program.data()), program.size());
    std::cout << "Assembled " << (program.size() - 2) << " bytes." << std::endl;

    return 0;
}