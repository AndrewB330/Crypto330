#include <iostream>
#include <fstream>
#include <chrono>

#include <crypto330/block/aes.hpp>
#include <crypto330/utils.hpp>
#include <crypto330/block/kalyna.hpp>

uint8_t get_char_byte(char c) {
    uint8_t res;
    *((char*)&res) = c;
    return res;
}

int main() {
    char x = -1;
    uint8_t y = x;
    std::cout << std::hex << (int)(get_char_byte(x)) << std::endl;
    std::cout << std::hex << (int)(y) << std::endl;
    std::cout << std::hex << (int)(get_char_byte(x >> 1)) << std::endl;
    std::cout << std::hex << (int)(y >> 1) << std::endl;
    return 0;
}
