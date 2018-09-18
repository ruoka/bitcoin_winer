#pragma once
#include <cstdint>
#include <type_traits>
#include <vector>
#include "gsl/span.hpp"
#include "cryptic/sha2.hpp"

// The hashes are in internal byte order; the other values are all in little-endian order.
// https://bitcoin.org/en/developer-reference#block-headers

namespace bitcoin
{

using byte = std::byte;

template<typename Native, std::size_t N = sizeof(Native)>
union integer
{
    static_assert(std::is_integral_v<Native>);
    using native_type = Native;
    integer() = default;
    constexpr integer(const native_type& number) : m_native(number)
    {}
    operator native_type () const
    {
        return m_native;
    }
    bool operator == (const integer& other) const
    {
        return m_native == other.m_native;
    }
    static constexpr std::size_t size() noexcept
    {
        return N;
    }
    auto as_bytes()
    {
        return gsl::make_span(m_bytes);
    }
    auto as_bytes() const
    {
        return gsl::make_span(m_bytes);
    }
private:
    native_type m_native = {0x0};
    byte m_bytes[size()];
};

using satoshis = integer<std::uint64_t>;

using signed_integer = integer<std::int32_t>;

using unsigned_integer = integer<std::uint32_t>;

using unsigned_long = integer<std::uint64_t>;

class variable_length_integer
{
public:
    variable_length_integer(std::uint64_t i)
    {
        if(i < 0xFDu)
        {
            m_uint8 = static_cast<std::uint8_t>(i);
            m_uint64 = 0x0;
        }
        else if(i <= 0xFFFFu)
        {
            m_uint8 = 0xFDu;
            m_uint16 = static_cast<std::uint16_t>(i);
        }
        else if(i <= 0xFFFFFFFFu)
        {
            m_uint8 = 0xFEu;
            m_uint32 = static_cast<std::uint32_t>(i);
        }
        else
        {
            m_uint8 = 0xFFu;
            m_uint64 = i;
        }
    }
    operator std::size_t () const
    {
        switch(m_uint8)
        {
            case 0xFDu: return m_uint16;
            case 0xFEu: return m_uint32;
            case 0xFFu: return m_uint64;
            default: return m_uint8;
        }
    }
    bool operator == (std::size_t number) const
    {
        switch(m_uint8)
        {
            case 0xFDu: return m_uint16 == number;
            case 0xFEu: return m_uint32 == number;
            case 0xFFu: return m_uint64 == number;
            default:    return m_uint8  == number;
        }
    }
    std::size_t size() const
    {
        switch(m_uint8)
        {
            case 0xFDu: return sizeof m_uint16;
            case 0xFEu: return sizeof m_uint32;
            case 0xFFu: return sizeof m_uint64;
            default:    return sizeof 0;
        }
    }
    auto as_byte()
    {
        return gsl::make_span(&m_byte,1);
    }
    auto as_byte() const
    {
        return gsl::make_span(&m_byte,1);
    }
    auto as_bytes()
    {
        return gsl::make_span(m_bytes,size());
    }
    auto as_bytes() const
    {
        return gsl::make_span(m_bytes,size());
    }
private:
    union
    {
        std::uint8_t m_uint8;
        std::byte m_byte;
    };
    union
    {
        std::uint16_t m_uint16;
        std::uint32_t m_uint32;
        std::uint64_t m_uint64;
        std::byte m_bytes[8];
    };
};

template<typename T>
using vector = std::vector<T>;

using script = vector<byte>;

using raw_transaction = vector<byte>;

class hash
{
public:
    hash() noexcept
    {
        std::fill(m_bytes, m_bytes + 32, byte{'\0'});
    }
    auto& operator = (const cryptic::sha256& sha) noexcept
    {
        sha.encode(m_bytes);
        return *this;
    }
    auto as_bytes() noexcept
    {
        return gsl::make_span(m_bytes);
    }
    auto as_bytes() const noexcept
    {
        return gsl::make_span(m_bytes);
    }
private:
    byte m_bytes[32] = {};
};

} // namespace bitcoin
