#include <gtest/gtest.h>
#include "bitcoin/miner.hpp"

TEST(BitcoinMiner,AdHoc)
{
    bitcoin::mine();
}

TEST(BitcoinMiner,Pool)
{
    bitcoin::pool_mine();
}
