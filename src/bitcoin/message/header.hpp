#pragma once
#include <iostream>
#include "bitcoin/message/payload.hpp"

namespace bitcoin::message
{

constexpr auto main	    = unsigned_integer{0xd9b4bef9u};
constexpr auto testnet  = unsigned_integer{0xdab5bffau};
constexpr auto testnet3 = unsigned_integer{0x0709110bu};
constexpr auto namecoin = unsigned_integer{0xfeb4bef9u};

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

} // namespace bitcoin::message

namespace std
{

inline auto& operator << (std::ostream& os, bitcoin::message::header& header)
{
    auto bos = bitcoin::message::obytestream{os};
    bos.write(header.magic.bytes);
    bos.write(header.command);
    bos.write(header.payload_length.bytes);
    bos.write(header.checksum.bytes);
    return os;
}

inline auto& operator >> (std::istream& is, bitcoin::message::header& header)
{
    auto bis = bitcoin::message::ibytestream{is};
    bis.read(header.magic.bytes);
    bis.read(header.command);
    bis.read(header.payload_length.bytes);
    bis.read(header.checksum.bytes);
    return is;
}

} // namespace std
