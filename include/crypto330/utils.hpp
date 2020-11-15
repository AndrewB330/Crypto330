#pragma once

#include <cstdint>
#include <string>
#include <vector>

uint8_t GF8_Mul(uint8_t a, uint8_t b, uint8_t poly);

std::vector<uint8_t> HexStringToBytes(const std::string &str);

std::vector<uint8_t> StringToBytes(const std::string &str);