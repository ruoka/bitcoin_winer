#pragma once
#include <iostream>
#include "bitcoin/message/payload.hpp"

namespace bitcoin::message
{

constexpr auto main	    = unsigned_integer{0xd9b4bef9u}; // FIXME, are these correct?
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
    auto command() const
    {
        return m_command;
    }
    auto payload_length() const
    {
        return m_payload_length;
    }
    auto payload_length(std::uint32_t length)
    {
        m_payload_length = length;
    }
    auto checksum() const
    {
        return m_checksum;
    }
    void dump(obytestream obs) const
    {
        obs.write(m_magic.as_bytes());
        obs.write(m_command.as_bytes());
        obs.write(m_payload_length.as_bytes());
        obs.write(m_checksum.as_bytes());
    }
    void pump(ibytestream ibs)
    {
        ibs.read(m_magic.as_bytes());
        ibs.read(m_command.as_bytes());
        ibs.read(m_payload_length.as_bytes());
        ibs.read(m_checksum.as_bytes());
    }
private:
    unsigned_integer m_magic = {main};
    message::command m_command = {};
    unsigned_integer m_payload_length = {0x0u};
    unsigned_integer m_checksum = {0x5df6e0e2u};
};

} // namespace bitcoin::message

namespace std
{

inline auto& operator << (std::ostream& os, const bitcoin::message::header& header)
{
    header.dump(os);
    return os;
}

inline auto& operator >> (std::istream& is, bitcoin::message::header& header)
{
    header.pump(is);
    return is;
}

} // namespace std
