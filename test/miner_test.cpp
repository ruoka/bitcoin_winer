#include <gtest/gtest.h>
#include "bitcoin/miner.hpp"

TEST(BitcoinMiner,Solo)
{
    const auto timestamp = std::chrono::system_clock::now();
    auto block = bitcoin::block{};
    auto hash = cryptic::sha256{};
    hash.encode(block.header.previous_block.as_bytes());
    hash.encode(block.header.merkle_root.as_bytes());
    block.header.timestamp = std::chrono::duration_cast<std::chrono::seconds>(timestamp.time_since_epoch()).count();
    block.header.bits = 0x1d00fffful;
    // block.header.bits = 0x172819a1;
    // block.header.bits = 0x1e00fffful;
    bitcoin::mine(block);
}

TEST(BitcoinMiner,Pool)
{
    const auto timestamp = std::chrono::system_clock::now();
    auto block = bitcoin::block{};
    auto hash = cryptic::sha256{};
    hash.encode(block.header.previous_block.as_bytes());
    hash.encode(block.header.merkle_root.as_bytes());
    block.header.timestamp = std::chrono::duration_cast<std::chrono::seconds>(timestamp.time_since_epoch()).count();
    block.header.bits = 0x1d00fffful;
    // block.header.bits = 0x172819a1;
    // block.header.bits = 0x1e00fffful;
    bitcoin::pool_mine(block);
}
