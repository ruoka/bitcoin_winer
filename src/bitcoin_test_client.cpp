#include <iostream>
#include <chrono>
#include "bitcoin/message/header.hpp"
#include "bitcoin/message/payload.hpp"
#include "net/connector.hpp"

using namespace bitcoin;

using namespace std::literals;

int main()
try {

    auto connection = net::connect("localhost", "18333");

    // Call a node connect
    {
        auto version = message::version{};
        auto payload = message::payload{version};
        auto header = message::header{payload};
        connection << header << payload << net::flush;
    }

    auto checks = 0;

    // Shake hands
    while(connection && checks < 2)
    {
        auto header = message::header{};
        connection >> header;
        auto payload = message::payload(header.payload_length());
        connection >> payload;

        if(header.is_version()) // version
        {
            auto version = message::version{};
            payload >> version;
            auto verack = message::verack{};
            auto payload = message::payload{verack};
            auto header = message::header{payload};
            connection << header << payload << net::flush;
            ++checks;
        }
        else if(header.is_verack()) // verack
        {
            auto verack = message::verack{};
            payload >> verack;
            ++checks;
        }
    }

    auto pings = 0;

    // Ping pong
    while(connection && pings < 2)
    {
        if(connection.wait_for(30s))
        {
            auto header = message::header{};
            connection >> header;
            auto payload = message::payload(header.payload_length());
            connection >> payload;

            if(header.is_ping()) // ping
            {
                auto ping = message::ping{};
                payload >> ping;
                auto pong = message::pong{ping};
                auto payload = message::payload{pong};
                auto header = message::header{payload};
                connection << header << payload << net::flush;
            }
            else if(header.is_pong()) // pong
            {
                auto pong = message::pong{};
                payload >> pong;
                --pings;
            }
        }
        else
        {
            auto ping = message::ping{};
            auto payload = message::payload{ping};
            auto header = message::header{payload};
            connection << header << payload << net::flush;
            ++pings;
        }
    }

    return 0;
}
catch(const std::exception& e)
{
    std::cerr << "Exception: " << e.what() << std::endl;
}
