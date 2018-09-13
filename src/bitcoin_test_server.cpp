#include <iostream>
#include <chrono>
#include "bitcoin/message/header.hpp"
#include "bitcoin/message/payload.hpp"
#include "net/acceptor.hpp"
#include "net/syslogstream.hpp"

using namespace bitcoin;
using namespace std::literals;
using namespace net;

int main()
try {

    net::slog.tag("WinerServer");
    slog.facility(syslog::facility::local0);
    slog.level(syslog::severity::debug);
    slog.redirect(std::clog);

    slog << debug << "Creating peer" << flush;
    auto acceptor = net::acceptor{"localhost", "18333"};
    slog << info << "Accepting connections" << flush;
    auto connection = acceptor.accept();
    slog << notice << "Accepted peer" << flush;

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

    // Ping pong
    while(connection && pings < 2)
    {
        if(connection.wait_for(30s))
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
