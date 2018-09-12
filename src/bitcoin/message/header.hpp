#pragma once
#include <iostream>
#include "bitcoin/message/payload.hpp"

namespace bitcoin::message
{

constexpr auto main	    = unsigned_integer{0xd9b4bef9u};
constexpr auto testnet  = unsigned_integer{0xdab5bffau};
constexpr auto testnet3 = unsigned_integer{0x0709110bu};
constexpr auto namecoin = unsigned_integer{0xfeb4bef9u};

class header
{
public:
    header() = default;
    header(const payload& pl) :
        m_command{pl.command()},
        m_payload_length{pl.length()},
        m_checksum{pl.checksum()}
    {}
    void write(obytestream obs)
    {
        obs.write(m_magic.bytes);
        obs.write(m_command);
        obs.write(m_payload_length.bytes);
        obs.write(m_checksum.bytes);
    }
    void read(ibytestream ibs)
    {
        ibs.read(m_magic.bytes);
        ibs.read(m_command);
        ibs.read(m_payload_length.bytes);
        ibs.read(m_checksum.bytes);
    }
    bool is_version() const
    {
        return false;
    }
    bool is_verack() const
    {
        return false;
    }
    bool is_ping() const
    {
        return false;
    }
    bool is_pong() const
    {
        return false;
    }
    unsigned_integer payload_length() const
    {
        return m_payload_length;
    }
    unsigned_integer checksum() const
    {
        return m_checksum;
    }
private:
    unsigned_integer m_magic = {main};
    std::array<byte,12> m_command = {};
    unsigned_integer m_payload_length = {0x0u};
    unsigned_integer m_checksum = {0x5df6e0e2u};
};

} // namespace bitcoin::message

namespace std
{

inline auto& operator << (std::ostream& os, bitcoin::message::header& header)
{
    header.write(os);
    return os;
}

inline auto& operator >> (std::istream& is, bitcoin::message::header& header)
{
    header.read(is);
    return is;
}

} // namespace std
