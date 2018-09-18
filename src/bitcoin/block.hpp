#pragma once
#include "bitcoin/transaction.hpp"

namespace bitcoin
{

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

    hash target() const
    {
        auto target = hash{};
        auto digits = (header.bits >> 24);
        auto coefficient = header.bits.as_bytes().first(3);
        auto value = target.as_bytes().last(digits);
        std::copy(coefficient.rbegin(), coefficient.rend(), value.begin());
        return target;
    }
};

} // namespace bitcoin
