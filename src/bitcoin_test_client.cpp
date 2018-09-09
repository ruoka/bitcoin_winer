#include <iostream>
#include <chrono>
#include "bitcoin/p2p/header.hpp"
#include "bitcoin/p2p/payload.hpp"
#include "net/connector.hpp"

using namespace bitcoin::p2p;

using namespace std::literals;

int main()
try {

    auto connection = net::connect("localhost", "18333");

    // Once
    {
        auto version = message::version{};
        auto payload = message::payload{version};
        auto header = message::header{payload};
        connection << header << payload;
    }

    while(connection)
    {
        auto header = message::header{};
        connection >> header;
        auto payload = message::payload(header.payload_length);
        connection >> payload;

        if(header.command == header.command) // version
        {
            auto verack = message::verack{};
            auto payload = message::payload{verack};
            auto header = message::header{payload};
            connection << header << payload;
        }
        else if(header.command == header.command) // verack
        {
            break;
        }
    }

    connection.wait_for(30s);

    while(connection)
    {
        auto header = message::header{};
        connection >> header;
        auto payload = message::payload(header.payload_length);
        connection >> payload;

        if(header.command == header.command) // ping
        {
            auto ping = message::ping{};
            payload >> ping;
            auto pong = message::pong{ping};
            auto payload = message::payload{pong};
            auto header = message::header{payload};
            connection << header << payload;
        }
    }

    return 0;
}
catch(const std::exception& e)
{
    std::cerr << "Exception: " << e.what() << std::endl;
}
