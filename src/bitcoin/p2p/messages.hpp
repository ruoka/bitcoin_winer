#pragma once
#include "bitcoin/primitives.hpp"

namespace bitcoin::p2p
{
    constexpr unsigned main	    = {0xd9b4bef9u};
    constexpr unsigned testnet  = {0xdab5bffau};
    constexpr unsigned testnet3 = {0x0709110bu};
    constexpr unsigned namecoin = {0xfeb4bef9u};

    namespace message
    {

    struct header
    {
        unsigned_integer magic = {testnet};
        byte command[12] = {};
        unsigned_integer payload_length = {0x0u};
        unsigned_integer checksum = {0x5df6e0e2u};
    };

    struct getheaders
    {
        unsigned_integer version = {protocol_version};
        variable_length_integer hash_count = {0x0u};
        vector<hash> block_locator_hashes = {};
        hash hash_stop = {};
    };

    struct sendheaders
    {
        // empty
    };

    struct headers
    {
        variable_length_integer header_count = {0x0u};
        vector<block> headers; // 81 bytes
    };

    struct getblocks
    {
        unsigned_integer version = {protocol_version};
        variable_length_integer hash_count = {0x0u};
        vector<hash> block_locator_hashes = {};
        hash hash_stop = {};
    };

    struct mempool
    {
        // empty
    };

    struct inventory
    {
        enum : unsigned {
            error = 0u,
            msg_tx = 1u,
            msg_bock = 2u,
            msg_filtered_block = 3u,
            msg_cmptc_block = 4u
        };
        unsigned_integer type = {error};
        hash object = {};
    };

    struct inv
    {
        variable_length_integer inventory_count = {0x0u};
        vector<inventory> inventories = {};
    };

    struct getdata
    {
        variable_length_integer inventory_count = {0x0u};
        vector<inventory> inventories = {};
    };

    using block = bitcoin::block;

    using tx = bitcoin::transaction;

    struct notfound
    {
        variable_length_integer inventory_count = {0x0u};
        vector<inventory> inventories = {};
    };

// control messages

    struct network_address
    {
        unsigned_integer timestamp = {0x0u}; // network addresses are not prefixed with a timestamp in the version message.
        unsigned_long services = {0x0ul};
        byte address[16] = {};
        unsigned_short port = {0x0u};
    };

    using boolean = byte;

    struct version
    {
        unsigned_integer version = {protocol_version};
        unsigned_long services = {0x0ul};
        unsigned_integer timestamp = {0x0u};
        network_address recipient_address = {};
        network_address sender_address = {};
        unsigned_long nonce = {0x0ul};
        variable_length_string user_agent = {0x0u,{}};
        unsigned_integer start_height = {0x0u};
        boolean relay = byte{0b00000000};
    };

    struct verack
    {
        // empty
    };

    struct getaddr
    {
        // empty
    };

    struct addr
    {
        variable_length_integer address_count = {0x0u};
        vector<network_address> addresses = {};
    };

    struct ping
    {
        unsigned_long nonce = {0x0ul};
    };

    struct pong
    {
        unsigned_long nonce = {0x0ul};
    };

    struct reject
    {
        variable_length_string message = {0x0u,{}};
        byte code = {};
        variable_length_string reason = {0x0u,{}};
        hash data = {};
    };

/*
    struct filteradd
    {};

    struct filterclear
    {};

    struct filterload
    {};

    struct merkleblock
    {};

    // protocol version >= 70013
    struct feefilter
    {};

    // protocol version >= 70014
    struct sendcmpct
    {};

    struct getblocktxn
    {};

    struct blocktxn
    {};
*/

    }
}
