#include <iostream>
#include <chrono>
#include <map>
#include "bitcoin/message/header.hpp"
#include "bitcoin/message/payload.hpp"
#include "net/connector.hpp"
#include "net/syslogstream.hpp"

using namespace bitcoin;
using namespace std::literals;
using namespace net;

inline auto hash_previous_block(const bitcoin::message::payload& payload)
{
    return cryptic::sha256{payload.as_bytes().first<80>()};
}

inline auto hash_merkle_root(std::vector<cryptic::sha256>& hashes1)
{
    if(hashes1.size() % 2 == 1)
        hashes1.push_back(hashes1.back());

    auto hashes2 = std::vector<cryptic::sha256>{};

    for(auto i = 0u; i < hashes1.size(); i+=2u)
    {
        auto buffer = std::array<std::byte,64>{};
        hashes1[i].encode(gsl::make_span(buffer).first<32>());
        hashes1[i+1].encode(gsl::make_span(buffer).last<32>());
        hashes2.push_back(cryptic::sha256{buffer});
    }

    if(hashes2.size() == 1)
        return hashes2.front();
    else
        return hash_merkle_root(hashes2);
}

inline auto timestamp()
{
    using namespace std::chrono;
    return duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
}

inline auto mine(bitcoin::block&)
{
    return false;
}

int main()
try {

    net::slog.tag("WinerClient");
    slog.facility(syslog::facility::local0);
    slog.level(syslog::severity::debug);
    slog.redirect(std::clog);

    slog << debug << "Connecting peer" << flush;
    auto connection = net::connect("localhost", "18333");
    slog << notice << "Connected peer" << flush;

    // Call a node connect
    {
        slog << info << "Sending version message" << flush;
        auto version = message::version{};
        auto payload = message::payload{version};
        auto header = message::header{payload};
        connection << header << payload << flush;
    }

    auto handshake = 0;

    // Shake hands
    while(connection && handshake < 2)
    {
        slog << debug << "Waiting for messages" << flush;
        auto header = message::header{};
        connection >> header;

        slog << debug << "Payload length: " << header.payload_length() << flush;

        auto payload = message::payload(header.payload_length());
        connection >> payload;

        if(header.command().is_version()) // version
        {
            slog << info << "Received version message" << flush;
            auto version = message::version{};
            payload >> version;
            slog << info << "Replying varack message" << flush;
            auto verack = message::verack{};
            auto payload = message::payload{verack};
            auto header = message::header{payload};
            connection << header << payload << flush;
            ++handshake;
        }
        else if(header.command().is_verack()) // verack
        {
            slog << info << "Received verack message" << flush;
            auto verack = message::verack{};
            payload >> verack;
            ++handshake;
        }
        else
        {
            slog << warning << "Received unknown message" << flush;
        }
    }

    auto pings = 0;

    auto transactions = std::map<cryptic::sha256,bitcoin::message::payload>{};

    // Ping pong
    while(connection && pings < 2)
    {
        if(connection.wait_for(25s))
        {
            slog << debug << "Waiting for messages" << flush;
            auto header = message::header{};
            connection >> header;

            slog << debug << "Payload length: " << header.payload_length() << flush;

            auto payload = message::payload(header.payload_length());
            connection >> payload;

            if(header.command().is_ping()) // ping
            {
                auto ping = message::ping{};
                payload >> ping;
                slog << info << "Received ping message: " << ping.nonce << flush;
                auto pong = message::pong{ping};
                auto payload = message::payload{pong};
                auto header = message::header{payload};
                connection << header << payload << flush;
                slog << info << "Replyed pong message: " << pong.nonce << flush;
            }
            else if(header.command().is_pong()) // pong
            {
                auto pong = message::pong{};
                payload >> pong;
                slog << info << "Received pong message: " << pong.nonce << flush;
                --pings;
            }
            else if(header.command().is_tx()) // tx
            {
                auto hash = cryptic::sha256{payload.as_bytes()};
                transactions.try_emplace(hash, payload);
                slog << info << "Received tx message" << flush;
            }
            else if(header.command().is_block()) // block
            {
                auto previous_block = bitcoin::block{};
                payload >> previous_block;
                slog << info << "Received block message" << flush;
                for(const auto& tx : previous_block.transactions)
                {
                    const auto raw_transaction = bitcoin::message::payload{tx};
                    const auto hash = cryptic::sha256{raw_transaction.as_bytes()};
                    transactions.erase(hash);
                }
                auto hashes = std::vector<cryptic::sha256>{};
                const auto coinbase = bitcoin::transaction::coinbase();
                const auto raw_transaction = bitcoin::message::payload{coinbase};
                const auto hash = cryptic::sha256{raw_transaction.as_bytes()};
                hashes.push_back(hash);
                for(const auto &hash : transactions)
                    hashes.push_back(hash.first);
                auto block = bitcoin::block{};
                block.header.version = previous_block.header.version;
                block.header.previous_block = hash_previous_block(payload);
                block.header.merkle_root = hash_merkle_root(hashes);
                block.header.timestamp = timestamp();
                block.header.bits = previous_block.header.bits;
                block.header.nonce = 0ul;
                block.transaction_count = 1u + transactions.size();
                if(mine(block))
                {
                    auto payload = message::payload{block};
                    payload << coinbase;
                    for(const auto &tx : transactions)
                        payload << tx.second;
                    const auto header = message::header{payload};
                    connection << header << payload << flush;
                    slog << info << "Sent block message" << flush;
                }
            }
            else
            {
                slog << warning << "Received unknown message" << flush;
            }
        }
        else
        {
            auto ping = message::ping{};
            auto payload = message::payload{ping};
            auto header = message::header{payload};
            connection << header << payload << flush;
            slog << info << "Sent ping message: " << ping.nonce << flush;
            ++pings;
        }
    }

    return 0;
}
catch(const std::exception& e)
{
    std::cerr << "Exception: " << e.what() << std::endl;
}
