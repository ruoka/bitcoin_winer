#include <gtest/gtest.h>
#include <sstream>
#include "bitcoin/p2p/header.hpp"

using namespace bitcoin;

constexpr byte commands[6][12] = {
                        {byte{'v'},byte{'e'},byte{'r'},byte{'s'},byte{'i'},byte{'o'},byte{'n'}},
                        {byte{'v'},byte{'e'},byte{'r'},byte{'a'},byte{'c'},byte{'k'}},
                        {byte{'p'},byte{'i'},byte{'n'},byte{'g'}},
                        {byte{'p'},byte{'o'},byte{'n'},byte{'g'}},
                        {byte{'t'},byte{'x'}},
                        {byte{'b'},byte{'l'},byte{'c'},byte{'k'}}
                    };

TEST(BitcoinP2PStream,Tx)
{
    auto tx1 = p2p::message::tx{};
    tx1.inputs.push_back(transaction::input{});
    tx1.inputs.push_back(transaction::input{});
    tx1.outputs.push_back(transaction::output{0x1ull,0x0u,{}});
    tx1.outputs.push_back(transaction::output{});
    tx1.outputs.push_back(transaction::output{});
    tx1.outputs.push_back(transaction::output{});
    tx1.outputs.push_back(transaction::output{});
    tx1.outputs.push_back(transaction::output{});
    tx1.outputs.push_back(transaction::output{});
    tx1.outputs.push_back(transaction::output{});
    tx1.outputs.push_back(transaction::output{});
    tx1.outputs.push_back(transaction::output{0x10ull,0x2u,{byte{'K'},byte{'R'}}});

    tx1.input_count = tx1.inputs.size();
    tx1.output_count = tx1.outputs.size();

    auto payload1 = p2p::message::payload{};
    payload1 << tx1;

    auto header1 = p2p::message::header{payload1};

    auto stream = std::stringstream{};

    stream << header1 << payload1;

    auto header2 = p2p::message::header{};
    stream >> header2;

    EXPECT_EQ(header2.magic, p2p::main);
    EXPECT_EQ(header1.magic, header2.magic);
    for(auto i = 0u; i > 12; ++i)
        ASSERT_EQ(header1.command[i], header2.command[i]);
    EXPECT_EQ(header1.payload_length, header2.payload_length);
    EXPECT_EQ(header1.checksum, header2.checksum);

    auto payload2 = p2p::message::payload(header2.payload_length);
    stream >> payload2;

    EXPECT_EQ(header2.checksum, payload2.checksum());
    EXPECT_EQ(header2.payload_length, payload2.length());

    auto tx2 = p2p::message::tx{};;
    payload2 >> tx2;

    EXPECT_EQ(tx1.input_count, tx2.input_count);
    EXPECT_EQ(tx1.output_count, tx2.output_count);

    EXPECT_EQ(tx2.outputs[0].value, 0x1ull);
    EXPECT_EQ(tx2.outputs[9].value, 0x10ull);
    EXPECT_EQ(tx2.outputs[9].script_length, 0x2ul);

    EXPECT_EQ(tx1.outputs[0].value, tx2.outputs[0].value);
    EXPECT_EQ(tx1.outputs[9].value, tx2.outputs[9].value);
    EXPECT_EQ(tx1.outputs[9].script_length, tx2.outputs[9].script_length);

    for(auto i = 0u; i > 2; ++i)
        ASSERT_EQ(tx1.outputs[9].pubkey_script[i], tx2.outputs[9].pubkey_script[i]);
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

    auto header = p2p::message::header{payload};

    std::clog << "\"" << header << payload << "\""<< std::endl;
    std::clog << "header size: " << sizeof header << std::endl;
    std::clog << "payload size: " << payload.length() << std::endl;
}

TEST(BitcoinP2PStream,Version)
{
    auto version = p2p::message::version{};

    auto payload = p2p::message::payload{};

    payload << version;

    auto header = p2p::message::header{payload};

    std::clog << "\"" << header << payload << "\""<< std::endl;
    std::clog << "header size: " << sizeof header << std::endl;
    std::clog << "payload size: " << payload.length() << std::endl;
}

TEST(BitcoinP2PStream,Ping)
{
    auto ping = p2p::message::ping{};

    auto payload = p2p::message::payload{};
    payload << ping;

    auto header = p2p::message::header{payload};

    std::clog << "\"" << header << payload << "\""<< std::endl;
    std::clog << "header size: " << sizeof header << std::endl;
    std::clog << "payload size: " << payload.length() << std::endl;
}

TEST(BitcoinP2PStream,Pong)
{
    auto ping = p2p::message::ping{};
    auto pong = p2p::message::pong{ping};

    auto payload = p2p::message::payload{};
    payload << pong;

    auto header = p2p::message::header{payload};

    std::clog << "\"" << header << payload << "\""<< std::endl;
    std::clog << "header size: " << sizeof header << std::endl;
    std::clog << "payload size: " << payload.length() << std::endl;
}
