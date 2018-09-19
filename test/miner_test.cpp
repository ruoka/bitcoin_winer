#include <gtest/gtest.h>
#include "bitcoin/miner.hpp"
#include "std/extension.hpp"

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
    block.header.bits = 0x1d00fffful;
    // block.header.bits = 0x172819a1;
    // block.header.bits = 0x1e00fffful;
    bitcoin::mine(block);
}

TEST(BitcoinMiner,Pool)
{
    const auto bytes = cryptic::as_bytes(cryptic::make_span("Some junk to hash XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXDFGDGFGGAD asdg sda dsgasdDSG ad adsgA SDG asd !"));

    const auto start = std::chrono::system_clock::now();
    const auto epoch = std::chrono::duration_cast<std::chrono::seconds>(start.time_since_epoch()).count();
    std::clog << ext::to_iso8601(start) << std::endl;

    const auto n = std::thread::hardware_concurrency();
    std::cout << n << " concurrent threads are supported:\n";

    auto unconfirmed_transactions = std::map<cryptic::sha256,bitcoin::transaction>{};
    unconfirmed_transactions.emplace(cryptic::sha256{bytes},bitcoin::transaction{});
    auto proposed_block = bitcoin::block{};
    proposed_block.reward_and_fees(125ull);
    proposed_block.hash_previous_block(bytes);
    proposed_block.add_transactions_and_hash_merkle_root(unconfirmed_transactions);
    proposed_block.header.timestamp = epoch;
    // block.header.bits = 0x1e00000ful;
    // block.header.bits = 0x1d00fffful;
    // block.header.bits = 0x1d0ffffful;
    proposed_block.header.bits = 0x172819a1;
    // block.header.bits = 0x1e00fffful;
    bitcoin::pool_mine(proposed_block);

    const auto end = std::chrono::system_clock::now();
    std::clog << ext::to_iso8601(end) << std::endl;
    auto time = (end - start);
    const auto ms = std::chrono::duration_cast<std::chrono::minutes>(time);
    time -= ms;
    const auto ss = std::chrono::duration_cast<std::chrono::seconds>(time);
    std::clog << ms << " minutes " << ss << " seconds" << std::endl;
}
