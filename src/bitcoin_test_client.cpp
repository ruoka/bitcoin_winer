#include <iostream>
#include <chrono>
#include "bitcoin/message/header.hpp"
#include "bitcoin/message/payload.hpp"
#include "net/connector.hpp"
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

    slog << debug << "Connecting" << flush;
    auto connection = net::connect("localhost", "18333");
    slog << debug << "Connected" << flush;

    // Call a node connect
    {
        slog << debug << "Sending version message" << flush;
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
            slog << debug << "Received version message" << flush;
            auto version = message::version{};
            payload >> version;
            slog << debug << "Sending varack message" << flush;
            auto verack = message::verack{};
            auto payload = message::payload{verack};
            auto header = message::header{payload};
            connection << header << payload << flush;
            ++handshake;
        }
        else if(header.command().is_verack()) // verack
        {
            slog << debug << "Received verack message" << flush;
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
                slog << debug << "Received ping message" << flush;
                auto ping = message::ping{};
                payload >> ping;
                slog << debug << "Sending pong message" << flush;
                auto pong = message::pong{ping};
                auto payload = message::payload{pong};
                auto header = message::header{payload};
                connection << header << payload << flush;
            }
            else if(header.command().is_pong()) // pong
            {
                slog << debug << "Received pong message" << flush;
                auto pong = message::pong{};
                payload >> pong;
                --pings;
            }
            else
            {
                slog << warning << "Received unknown message" << flush;
            }
        }
        else
        {
            slog << debug << "Sending ping message" << flush;
            auto ping = message::ping{};
            auto payload = message::payload{ping};
            auto header = message::header{payload};
            connection << header << payload << flush;
            ++pings;
        }
    }

    return 0;
}
catch(const std::exception& e)
{
    std::cerr << "Exception: " << e.what() << std::endl;
}
