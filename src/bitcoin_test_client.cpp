#include <iostream>
#include <chrono>
#include <map>
#include "bitcoin/message/header.hpp"
#include "bitcoin/message/payload.hpp"
#include "net/connector.hpp"
#include "net/syslogstream.hpp"

// https://en.bitcoin.it/wiki/Transaction_fees

using namespace bitcoin;
using namespace std::literals;
using namespace net;

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

    auto transactions = std::map<cryptic::sha256,bitcoin::transaction>{};

    auto unconfirmed_transactions = std::map<cryptic::sha256,bitcoin::transaction>{};

    auto confirmed_transactions = std::map<cryptic::sha256,bitcoin::transaction>{};

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
                auto transaction = bitcoin::transaction{};
                payload >> transaction;
                transactions.try_emplace(hash, transaction);
                unconfirmed_transactions.try_emplace(hash, transaction);
                slog << info << "Received tx message" << flush;
            }
            else if(header.command().is_block()) // block
            {
                auto previous_block = bitcoin::block{};
                payload >> previous_block;
                slog << info << "Received block message" << flush;

                for(const auto& transaction : previous_block.transactions)
                {
                    const auto raw_transaction = bitcoin::message::payload{transaction};
                    const auto hash = cryptic::sha256{raw_transaction.as_bytes()};
                    transactions.try_emplace(hash, transaction);
                    confirmed_transactions.try_emplace(hash, transaction);
                    unconfirmed_transactions.erase(hash);
                }

                auto proposed_block = bitcoin::block{};
                proposed_block.hash_previous_block(payload.as_bytes());
                proposed_block.reward_and_fees(125ull);
                proposed_block.add_transactions_and_hash_merkle_root(unconfirmed_transactions);

                const auto success = mine(proposed_block);

                if(success)
                {
                    const auto payload = message::payload{proposed_block};
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
