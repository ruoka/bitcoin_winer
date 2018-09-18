#include <gtest/gtest.h>
#include <iostream>
#include "bitcoin/block.hpp"

// 0x00000000FFFF0000000000000000000000000000000000000000000000000000
//   00000000ffff0000000000000000000000000000000000000000000000000000
// 0000000000000000000000000000000011111111111111110000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000

TEST(BitcoinBlock,Target)
{
    std::cout << std::bitset<32>(0x1d00fffful) << std::endl;;
    std::cout << std::bitset<32>(0x1d00fffful >> 24) << std::endl;;
    std::cout << (0x1d00fffful >> 24) << std::endl;;

    // std::cout << std::bitset<32>(0x1e00000ful) << std::endl;;
    // std::cout << std::bitset<32>(0x1e00000ful >> 24) << std::endl;;
    // std::cout << (0x1e00000ful >> 24) << std::endl;;

    auto block = bitcoin::block{};
    block.header.bits = 0x1d00fffful;
    // block.header.bits = 0x172819a1;
    // block.header.bits = 0x1e00000ful;

    // std::clog << std::bitset<8>(0x1dul) << std::endl;

    auto hash = block.target();

    auto bytes = hash.as_bytes();

    for(auto i = 0u; i < 32u; ++i)
        std::clog << std::setw(2) << std::setfill('0') << std::hex << std::to_integer<std::uint32_t>(bytes[i]);
    std::clog << std::endl;

    for(auto i = 0u; i < 32u; ++i)
        std::clog << std::bitset<8>(std::to_integer<std::uint8_t>(bytes[i]));
    std::clog << std::endl;
}
