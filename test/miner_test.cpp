#include <gtest/gtest.h>
#include "bitcoin/miner.hpp"

TEST(BitcoinMiner,Solo)
{
    const auto timestamp = std::chrono::system_clock::now();
    auto block = bitcoin::block{};
    auto hash = cryptic::sha256{};
    hash.hash(as_bytes(cryptic::make_span("000000000000000000244ef253171aa35c126202b48a41c1e 6eeeae270df98f5cb6f4b8ece35c90ecfb142117120b530e8cd01497d1d0b0fb")));
    hash.encode(block.header.previous_block.as_bytes());
    hash.hash(as_bytes(cryptic::make_span("6eeeae270df98f5cb6f4b8ece35c90ecfb142117120b530e8cd01497d1d0b0fb 6eeeae270df98f5cb6f4b8ece35c90ecfb142117120b530e8cd01497d1d0b0fb")));
    hash.encode(block.header.merkle_root.as_bytes());
    block.header.timestamp = std::chrono::duration_cast<std::chrono::seconds>(timestamp.time_since_epoch()).count();
    // block.header.bits = 0x1d00fffful;
    block.header.bits = 0x172819a1;
    // block.header.bits = 0x1e00fffful;
    bitcoin::mine(block);
}

TEST(BitcoinMiner,Pool)
{
    unsigned int n = std::thread::hardware_concurrency();
    std::cout << n << " concurrent threads are supported:\n";

    const auto timestamp = std::chrono::system_clock::now();
    auto block = bitcoin::block{};
    auto hash = cryptic::sha256{};
    hash.hash(as_bytes(cryptic::make_span("000000000000000000244ef253171aa35c126202b48a41c1e 6eeeae270df98f5cb6f4b8ece35c90ecfb142117120b530e8cd01497d1d0b0fb")));
    hash.encode(block.header.previous_block.as_bytes());
    hash.hash(as_bytes(cryptic::make_span("6eeeae270df98f5cb6f4b8ece35c90ecfb142117120b530e8cd01497d1d0b0fb 6eeeae270df98f5cb6f4b8ece35c90ecfb142117120b530e8cd01497d1d0b0fb")));
    hash.encode(block.header.merkle_root.as_bytes());
    block.header.timestamp = std::chrono::duration_cast<std::chrono::seconds>(timestamp.time_since_epoch()).count();
    block.header.bits = 0x1d00fffful;
    // block.header.bits = 0x172819a1;
    // block.header.bits = 0x1e00fffful;
    bitcoin::pool_mine(block);
}
