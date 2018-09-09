#pragma once
#include <iostream>
#include "gsl/span.hpp"

namespace bitcoin::p2p::message
{

class obytestream
{
public:

    obytestream(std::ostream& os) : m_os{os}
    {}

    void write(const gsl::span<std::byte> bytes)
    {
        m_os.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
    }

private:

    std::ostream& m_os;
};

class ibytestream
{
public:

    ibytestream(std::istream& is) : m_is{is}
    {}

    void read(gsl::span<std::byte> bytes)
    {
        m_is.read(reinterpret_cast<char*>(bytes.data()), bytes.size());
    }

private:

    std::istream& m_is;
};

} // namespace bitcoin::p2p::message
