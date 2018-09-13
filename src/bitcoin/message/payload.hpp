#pragma once
#include "bitcoin/message/command.hpp"
#include "bitcoin/message/bytestream.hpp"
#include "cryptic/sha2.hpp"

using namespace std::literals;

namespace bitcoin::message
{

using sha032 = cryptic::sha2<0x6a09e667u,0xbb67ae85u,0x3c6ef372u,0xa54ff53au,0x510e527fu,0x9b05688cu,0x1f83d9abu,0x5be0cd19u,1>;

class payload
{
public:

    payload() : m_buffer{}, m_current{m_buffer.begin()}, m_command{}
    {}

    explicit payload(std::size_t size) : m_buffer(size), m_current{m_buffer.begin()}, m_command{}
    {}

    template<typename Message,
             typename = std::enable_if_t<!std::is_same_v<Message, unsigned_integer>>>
    explicit payload(const Message& msg) : m_buffer{}, m_current{m_buffer.begin()}, m_command{msg}
    {
        *this << msg;
    }

    auto command() const
    {
        return m_command;
    }

    auto length() const
    {
        return unsigned_integer{m_buffer.size()};
    }

    auto checksum() const
    {
        const auto hash = sha032{m_buffer};
        auto number = unsigned_integer{};
        hash.encode(number.as_bytes());
        return number;
    }

    auto& operator << (const message::version& msg)
    {
        Expects(msg.user_agent.length == msg.user_agent.string.size());
        write(msg.version);
        write(msg.services);
        write(msg.timestamp);
        write(msg.recipient_address.services);
        write(msg.recipient_address.address);
        write(msg.recipient_address.port);
        write(msg.sender_address.services);
        write(msg.sender_address.address);
        write(msg.sender_address.port);
        write(msg.nonce);
        write(msg.user_agent.length);
        write(msg.user_agent.string);
        write(msg.start_height);
        return *this;
    }

    auto& operator >> (message::version& msg)
    {
        read(msg.version);
        read(msg.services);
        read(msg.timestamp);
        read(msg.recipient_address.services);
        read(msg.recipient_address.address);
        read(msg.recipient_address.port);
        read(msg.sender_address.services);
        read(msg.sender_address.address);
        read(msg.sender_address.port);
        read(msg.nonce);
        read(msg.user_agent.length);
        msg.user_agent.string.resize(msg.user_agent.length);
        read(msg.user_agent.string);
        read(msg.start_height);
        Ensures(msg.user_agent.length == msg.user_agent.string.size());
        return *this;
    }

    auto& operator << (const message::verack& msg)
    {
        return *this;
    }

    auto& operator >> (message::verack& msg)
    {
        return *this;
    }

    auto& operator << (const message::ping& msg)
    {
        write(msg.nonce);
        return *this;
    }

    auto& operator >> (message::ping& msg)
    {
        read(msg.nonce);
        return *this;
    }

    auto& operator << (const message::pong& msg)
    {
        write(msg.nonce);
        return *this;
    }

    auto& operator >> (message::pong& msg)
    {
        read(msg.nonce);
        return *this;
    }

    auto& operator << (const message::tx& tx)
    {
        Expects(tx.input_count == tx.inputs.size());
        Expects(tx.output_count == tx.outputs.size());
        write(tx.version);
        write(tx.input_count);
        for(const auto& input : tx.inputs)
        {
            Expects(input.script_length == input.signature_script.size());
            write(input.previous_output.txid);
            write(input.previous_output.output_index);
            write(input.script_length);
            write(input.signature_script);
            write(input.sequence);
        }
        write(tx.output_count);
        for(const auto& output : tx.outputs)
        {
            Expects(output.script_length == output.pubkey_script.size());
            write(output.value);
            write(output.script_length);
            write(output.pubkey_script);
        }
        write(tx.lock_time);
        return *this;
    }

    auto& operator >> (message::tx& tx)
    {
        read(tx.version);
        read(tx.input_count);
        tx.inputs.resize(tx.input_count);
        for(auto& input : tx.inputs)
        {
            read(input.previous_output.txid);
            read(input.previous_output.output_index);
            read(input.script_length);
            input.signature_script.resize(input.script_length);
            read(input.signature_script);
            read(input.sequence);
            Ensures(input.script_length == input.signature_script.size());
        }
        read(tx.output_count);
        tx.outputs.resize(tx.output_count);
        for(auto& output : tx.outputs)
        {
            read(output.value);
            read(output.script_length);
            output.pubkey_script.resize(output.script_length);
            read(output.pubkey_script);
            Ensures(output.script_length == output.pubkey_script.size());
        }
        read(tx.lock_time);
        Ensures(tx.input_count == tx.inputs.size());
        Ensures(tx.output_count == tx.outputs.size());
        return *this;
    }

    auto& operator << (const message::block& block)
    {
        Expects(block.transaction_count == block.transactions.size());
        write(block.header.version);
        write(block.header.previous_block);
        write(block.header.merkle_root);
        write(block.header.timestamp);
        write(block.header.bits);
        write(block.header.nonce);
        write(block.transaction_count);
        for(const auto& tx : block.transactions)
            *this << tx;
        return *this;
    }

    auto& operator >> (message::block& block)
    {
        read(block.header.version);
        read(block.header.previous_block);
        read(block.header.merkle_root);
        read(block.header.timestamp);
        read(block.header.bits);
        read(block.header.nonce);
        read(block.transaction_count);
        block.transactions.resize(block.transaction_count);
        for(auto& tx : block.transactions)
            *this >> tx;
        Ensures(block.transaction_count == block.transactions.size());
        return *this;
    }

    void dump(obytestream obs) const
    {
        obs.write(m_buffer);
    }

    void pump(ibytestream ibs)
    {
        ibs.read(m_buffer);
    }

private:

    void write(const gsl::span<const byte> bytes)
    {
        m_buffer.insert(m_buffer.end(), bytes.begin(), bytes.end());
    }

    void read(gsl::span<byte> bytes)
    {
        std::copy_n(m_current, bytes.size(), bytes.begin());
        std::advance(m_current, bytes.size());
    }

    template<typename Native>
    void write(const integer<Native>& number)
    {
        write(number.as_bytes());
    }

    template<typename Native>
    void read(integer<Native>& number)
    {
        read(number.as_bytes());
    }

    void write(const variable_length_integer& number)
    {
        write(number.as_byte());
        write(number.as_bytes());
    }

    void read(variable_length_integer& number)
    {
        read(number.as_byte());
        read(number.as_bytes());
    }

    void write(const hash& number)
    {
        write(number.as_bytes());
    }

    void read(hash& number)
    {
        read(number.as_bytes());
    }

    vector<byte> m_buffer;

    vector<byte>::const_iterator m_current;

    message::command m_command;
};

} // namespace bitcoin::message

namespace std {

    inline auto& operator << (std::ostream& os, const bitcoin::message::payload& payload)
    {
        payload.dump(os);
        return os;
    }

    inline auto& operator >> (std::istream& is, bitcoin::message::payload& payload)
    {
        payload.pump(is);
        return is;
    }

} // namespace std
