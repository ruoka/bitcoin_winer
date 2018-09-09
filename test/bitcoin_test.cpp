#include <gtest/gtest.h>
#include <iostream>
#include "bitcoin/log/stream.hpp"
#include "bitcoin/p2p/payload.hpp"

using namespace bitcoin;

// TEST(BitcoinP2PStream,AdHoc)
// {
//     auto stream = p2p::stream{std::clog};
//
//     auto header = p2p::message::header{};
//
//     stream >> header;
//
//     if(header.command[0] == std::byte{'t'}) // tx
//     {
//         auto transaction = p2p::message::tx{};
//         stream >> transaction;
//     }
//
//     {
//         auto transaction = p2p::message::tx{};
//         stream << header;
//         stream << transaction;
//     }
// }
//
// TEST(BitcoinLogStream,AdHoc)
// {
//     auto block = bitcoin::block{};
//     block.header.version = {70001u};
//
//     auto stream = log::stream{std::clog};
//     stream << block.header.version << log::flush;
//
//     std::clog << std::endl;
// };
