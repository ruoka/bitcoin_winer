#pragma once
#include "bitcoin/primitives.hpp"

namespace bitcoin
{

// https://en.bitcoin.it/wiki/Genesis_block

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
        unsigned_integer sequence = {0xffffffffu};
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

    static auto coinbase()
    {
        return transaction{}; // FIXME, add public kay and reward!
    }
};

} // namespace bitcoin
