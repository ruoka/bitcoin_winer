#pragma once
#include <cstdint>
#include <type_traits>
#include <vector>

// The hashes are in internal byte order; the other values are all in little-endian order.
// https://bitcoin.org/en/developer-reference#block-headers

namespace bitcoin
{
    using byte = std::byte;

    template<typename Native>
    union integer
    {
        static_assert(std::is_integral_v<Native>);
        Native native;
        byte bytes[sizeof(Native)];
        operator std::size_t () const
        {
            return std::size_t{native};
        }
        bool operator == (const integer& other) const
        {
            return native == other.native;
        }
        bool operator == (const Native& other) const
        {
            return native == other;
        }
    };

    using satoshis = integer<std::uint64_t>;

    using signed_integer = integer<std::int32_t>;

    using unsigned_integer = integer<std::uint32_t>;

    using unsigned_long = integer<std::uint64_t>;

    struct variable_length_integer
    {
        union
        {
            std::uint8_t uint8;
            std::byte byte;
        };
        union
        {
            std::uint16_t uint16;
            std::uint32_t uint32;
            std::uint64_t uint64;
            std::byte bytes[8];
        };

        variable_length_integer(std::uint64_t i)
        {
            if(i < 0xFDu)
            {
                uint8 = static_cast<std::uint8_t>(i);
                uint64 = 0x0;
            }
            else if(i <= 0xFFFFu)
            {
                uint8 = 0xFDu;
                uint16 = static_cast<std::uint16_t>(i);
            }
            else if(i <= 0xFFFFFFFFu)
            {
                uint8 = 0xFEu;
                uint32 = static_cast<std::uint32_t>(i);
            }
            else
            {
                uint8 = 0xFFu;
                uint64 = i;
            }
        }

        operator std::size_t () const
        {
            switch(uint8)
            {
                case 0xFDu: return uint16;
                case 0xFEu: return uint32;
                case 0xFFu: return uint64;
                default: return uint8;
            }
        }

        bool operator == (std::size_t size) const
        {
            switch(uint8)
            {
                case 0xFDu: return uint16 == size;
                case 0xFEu: return uint32 == size;
                case 0xFFu: return uint64 == size;
                default: return uint8 == size;
            }
        }
    };

    template<typename T>
    using vector = std::vector<T>;

    using script = vector<byte>;

    using raw_transaction = vector<byte>;

    struct hash
    {
        byte bytes[32] = {};
    };

    struct transaction
    {
        struct outpoint
        {
            hash txid = {};
            unsigned_integer output_index = {0x0u};
        };
        struct input
        {
            outpoint previous_output = {};
            variable_length_integer script_length = {0x0u};
            script signature_script = {};
            unsigned_integer sequence = {0x0u};
        };
        struct output
        {
            satoshis value = {0x0u};
            variable_length_integer script_length = {0x0u};
            script pubkey_script = {};
        };
        signed_integer version = {0x1}; // NOTE this is signed
        variable_length_integer input_count = {0x0u};
        vector<input> inputs = {};
        variable_length_integer output_count = {0x0u};
        vector<output> outputs = {};
        unsigned_integer lock_time = {};
    };

    struct block
    {
        struct
        {
            signed_integer version = {0x1}; // NOTE this is signed
            hash previous_block = {};
            hash merkle_root = {};
            unsigned_integer timestamp = {0x0u};
            unsigned_integer bits = {0x0u};
            unsigned_integer nonce = {0x0u};
        } header;
        variable_length_integer transaction_count = {0x0u};
        vector<transaction> transactions = {};
    };
}
