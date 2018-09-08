#pragma once
#include <sstream>
#include "gsl/assert.hpp"
#include "bitcoin/p2p/messages.hpp"

namespace bitcoin::p2p
{

class obytestream
{
public:

    obytestream(std::ostream& os) : m_os{os}
    {}

    template<std::size_t N>
    void write(const byte (&bytes)[N], const std::size_t size = N)
    {
        m_os.write(reinterpret_cast<const char*>(bytes), size);
    }

private:

    std::ostream& m_os;
};

class ibytestream
{
public:

    ibytestream(std::istream& is) : m_is{is}
    {}

    template<std::size_t N>
    void read(byte (&bytes)[N], const std::size_t size = N)
    {
        m_is.read(reinterpret_cast<char*>(bytes), size);
    }

private:

    std::istream& m_is;
};

} // namespace bitcoin::p2p

namespace std
{

inline auto& operator << (std::ostream& os, const bitcoin::p2p::message::header& header)
{
    auto bos = bitcoin::p2p::obytestream{os};
    bos.write(header.magic.bytes);
    bos.write(header.command);
    bos.write(header.payload_length.bytes);
    bos.write(header.checksum.bytes);
    return os;
}

inline auto& operator >> (std::istream& is, bitcoin::p2p::message::header& header)
{
    auto bis = bitcoin::p2p::ibytestream{is};
    bis.read(header.magic.bytes);
    bis.read(header.command);
    bis.read(header.payload_length.bytes);
    bis.read(header.checksum.bytes);
    return is;
}

} // namespace std
