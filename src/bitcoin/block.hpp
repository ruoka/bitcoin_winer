#pragma once
#include "bitcoin/transaction.hpp"

namespace bitcoin
{

inline auto hash_merkle_root(std::vector<cryptic::sha256>& txids)
{
    if(txids.size() % 2 == 1)
        txids.push_back(txids.back());
    auto hashes = std::vector<cryptic::sha256>{};
    for(auto i = 0u; i < txids.size(); i+=2u)
    {
        auto pair = std::array<std::byte,64>{};
        txids[i].encode(gsl::make_span(pair).first<32>());
        txids[i+1].encode(gsl::make_span(pair).last<32>());
        hashes.push_back(cryptic::sha256{pair});
    }
    if(hashes.size() == 1)
        return hashes.front();
    else
        return hash_merkle_root(hashes);
}

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

    void hash_previous_block(const gsl::span<byte> previous_block)
    {
        header.previous_block = cryptic::sha256{previous_block.first<80>()};
    }

    void reward_and_fees(const bitcoin::satoshis& value)
    {
        transactions.push_back(bitcoin::transaction::coinbase());
        transactions.front().outputs.front().value = value;
    }

    void add_transactions_and_hash_merkle_root(const std::map<cryptic::sha256,bitcoin::transaction> txs)
    {
        auto txids = std::vector<cryptic::sha256>{};
        for(const auto &tx : txs)
        {
            txids.push_back(tx.first);
            transactions.push_back(tx.second);
        }
        header.merkle_root = hash_merkle_root(txids);
        transaction_count = transactions.size();
    }

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
