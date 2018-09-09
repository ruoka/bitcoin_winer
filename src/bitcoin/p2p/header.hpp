#pragma once
#include <iostream>
#include "bitcoin/p2p/payload.hpp"

namespace bitcoin::p2p::message
{

struct header
{
    header() = default;

    header(const payload& pl) :
        command{pl.command()},
        payload_length{pl.length()},
        checksum{pl.checksum()}
    {}

    unsigned_integer magic = {main};
    std::array<byte,12> command = {};
    unsigned_integer payload_length = {0x0u};
    unsigned_integer checksum = {0x5df6e0e2u};
};

} // namespace bitcoin::p2p::message

namespace std
{

inline auto& operator << (std::ostream& os, bitcoin::p2p::message::header& header)
{
    auto bos = bitcoin::p2p::message::obytestream{os};
    bos.write(header.magic.bytes);
    bos.write(header.command);
    bos.write(header.payload_length.bytes);
    bos.write(header.checksum.bytes);
    return os;
}

inline auto& operator >> (std::istream& is, bitcoin::p2p::message::header& header)
{
    auto bis = bitcoin::p2p::message::ibytestream{is};
    bis.read(header.magic.bytes);
    bis.read(header.command);
    bis.read(header.payload_length.bytes);
    bis.read(header.checksum.bytes);
    return is;
}

} // namespace std
