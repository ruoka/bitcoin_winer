#include <gtest/gtest.h>
#include "bitcoin/p2p/payload.hpp"

using namespace bitcoin;

TEST(BitcoinP2PStream,Tx)
{
    auto tx = p2p::message::tx{};
    tx.inputs.push_back(transaction::input{});
    tx.outputs.push_back(transaction::output{});
    tx.outputs.push_back(transaction::output{});
    tx.outputs.push_back(transaction::output{});
    tx.outputs.push_back(transaction::output{});
    tx.outputs.push_back(transaction::output{});
    tx.outputs.push_back(transaction::output{});
    tx.outputs.push_back(transaction::output{});
    tx.outputs.push_back(transaction::output{});
    tx.outputs.push_back(transaction::output{});
    tx.outputs.push_back(transaction::output{});
    tx.input_count = tx.inputs.size();
    tx.output_count = tx.outputs.size();

    auto payload = p2p::message::payload{};
    payload << tx;

    auto header = p2p::message::header
                    {
                        p2p::main,
                        {byte{'t'},byte{'x'}},
                        payload.length(),
                        payload.checksum()
                    };

    std::clog << "\"" << header << payload << "\""<< std::endl;
    std::clog << "header size: " << sizeof header << std::endl;
    std::clog << "payload size: " << payload.length() << std::endl;
}

TEST(BitcoinP2PStream,Block)
{
    auto block = p2p::message::block{};
    block.transactions.push_back(transaction{});
    block.transactions.push_back(transaction{});
    block.transactions.push_back(transaction{});
    block.transaction_count = block.transactions.size();

    auto payload = p2p::message::payload{};
    payload << block;

    auto header = p2p::message::header
                    {
                        p2p::main,
                        {byte{'b'},byte{'l'},byte{'o'},byte{'c'},byte{'k'}},
                        payload.length(),
                        payload.checksum()
                    };

    std::clog << "\"" << header << payload << "\""<< std::endl;
    std::clog << "header size: " << sizeof header << std::endl;
    std::clog << "payload size: " << payload.length() << std::endl;
}

TEST(BitcoinP2PStream,Version)
{
    auto version = p2p::message::version{};

    auto payload = p2p::message::payload{};
    payload << version;

    auto header = p2p::message::header
                    {
                        p2p::main,
                        {byte{'v'},byte{'e'},byte{'r'},byte{'s'},byte{'i'},byte{'o'},byte{'n'}},
                        payload.length(),
                        payload.checksum()
                    };

    std::clog << "\"" << header << payload << "\""<< std::endl;
    std::clog << "header size: " << sizeof header << std::endl;
    std::clog << "payload size: " << payload.length() << std::endl;
}

TEST(BitcoinP2PStream,Ping)
{
    auto ping = p2p::message::ping{};

    auto payload = p2p::message::payload{};
    payload << ping;

    auto header = p2p::message::header
                    {
                        p2p::main,
                        {byte{'p'},byte{'i'},byte{'n'},byte{'g'}},
                        payload.length(),
                        payload.checksum()
                    };

    std::clog << "\"" << header << payload << "\""<< std::endl;
    std::clog << "header size: " << sizeof header << std::endl;
    std::clog << "payload size: " << payload.length() << std::endl;
}

TEST(BitcoinP2PStream,Pong)
{
    auto pong = p2p::message::pong{};

    auto payload = p2p::message::payload{};
    payload << pong;

    auto header = p2p::message::header
                    {
                        p2p::main,
                        {byte{'p'},byte{'o'},byte{'n'},byte{'g'}},
                        payload.length(),
                        payload.checksum()
                    };

    std::clog << "\"" << header << payload << "\""<< std::endl;
    std::clog << "header size: " << sizeof header << std::endl;
    std::clog << "payload size: " << payload.length() << std::endl;
}
