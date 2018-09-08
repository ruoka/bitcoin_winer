#include <iostream>
#include "bitcoin/primitives.hpp"
#include "bitcoin/p2p/messages.hpp"
#include "bitcoin/p2p/stream.hpp"
#include "bitcoin/log/stream.hpp"

using namespace bitcoin;

void test1()
{
    auto stream = p2p::stream{std::clog};

    auto header = p2p::message::header{};

    stream >> header;

    if(header.command[0] == std::byte{'t'}) // tx
    {
        auto tx = p2p::message::tx{};
        stream >> tx;
    }

    auto tx = p2p::message::tx{};
    stream << header;
    stream << tx;
}

int main()
{
    auto block = bitcoin::block{};
    block.header.version = {70001u};

    auto stream = log::stream{std::clog};
    stream << block.header.version << log::flush;

    std::clog << std::endl;

    return 0;
};
