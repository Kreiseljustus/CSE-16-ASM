#pragma once

#include <cstdint>
#include <unordered_map>
#include <string>

std::unordered_map<std::string, uint16_t> registers = {
    {"A", 0}, {"B", 1}, {"C", 2}, {"D", 3}, {"SP", 4}, {"BP", 5 }
};