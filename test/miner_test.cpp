#include <gtest/gtest.h>
#include "bitcoin/miner.hpp"

TEST(BitcoinMiner,AdHoc)
{
    const auto timestamp = std::chrono::system_clock::now();
    bitcoin::mine(timestamp);
}

TEST(BitcoinMiner,Pool)
{
    const auto timestamp = std::chrono::system_clock::now();
    bitcoin::pool_mine(timestamp);
}
