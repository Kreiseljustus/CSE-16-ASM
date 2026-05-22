#pragma once

#include <unordered_map>
#include <string>
#include <cstdint>

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
    {"LDB", 0x1F},
    {"HLT",   0xFF}
};