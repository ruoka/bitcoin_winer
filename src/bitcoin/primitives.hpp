#pragma once
#include <cstdint>
#include <vector>

// The hashes are in internal byte order; the other values are all in little-endian order.
// https://bitcoin.org/en/developer-reference#block-headers

namespace bitcoin
{
    constexpr unsigned protocol_version = {70012u};

    union signed_integer
    {
        std::int32_t native;
        std::byte bytes[4];
    };

    union unsigned_short
    {
        std::int16_t native;
        std::byte bytes[2];
    };

    union unsigned_integer
    {
        std::uint32_t native;
        std::byte bytes[4];
    };

    union unsigned_long
    {
        std::uint64_t native;
        std::byte bytes[8];
    };

    struct variable_length_integer
    {
        std::uint8_t uint8;
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

        operator std::uint64_t ()
        {
            switch(uint8)
            {
                case 0xFDu: return uint16;
                case 0xFEu: return uint32;
                case 0xFFu: return uint64;
                default: return uint8;
            }
        }
    };

    using satoshis = unsigned_long;

    template<typename T>
    using vector = std::vector<T>;

    using byte = std::byte;

    using script = vector<byte>;

    using raw_transaction = vector<byte>;

    struct variable_length_string
    {
        variable_length_integer length = {0x0u};
        vector<byte> string = {};
    };

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
        signed_integer version = {1u};
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
            unsigned_integer version = {protocol_version};
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
