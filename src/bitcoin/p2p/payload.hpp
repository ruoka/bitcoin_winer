#pragma once
#include "bitcoin/p2p/bytestream.hpp"
#include "cryptic/sha2.hpp"

namespace bitcoin::p2p::message
{

using sha032 = cryptic::sha2<0x6a09e667u,0xbb67ae85u,0x3c6ef372u,0xa54ff53au,0x510e527fu,0x9b05688cu,0x1f83d9abu,0x5be0cd19u,1>;

class payload
{
public:

    payload() : m_buffer{}, m_current{m_buffer.begin()}
    {}

    auto length() const
    {
        return static_cast<std::uint32_t>(m_buffer.size());
    }

    auto checksum() const
    {
        if(length() == 0)
            return 0x5df6e0e2u;

        const auto hash = sha032{m_buffer};
        auto sum = unsigned_integer{};
        hash.encode(sum.bytes);
        return sum.native;
    }

    auto& operator << (const message::version& msg)
    {
        Expects(msg.user_agent.length == msg.user_agent.string.size());
        write(msg.version.bytes);
        write(msg.services.bytes);
        write(msg.timestamp.bytes);
        write(msg.recipient_address.services.bytes);
        write(msg.recipient_address.address);
        write(msg.recipient_address.port.bytes);
        write(msg.sender_address.services.bytes);
        write(msg.sender_address.address);
        write(msg.sender_address.port.bytes);
        write(msg.nonce.bytes);
        write(msg.user_agent.length);
        write(msg.user_agent.string);
        write(msg.start_height.bytes);
        return *this;
    }

    auto& operator >> (message::version& msg)
    {
        read(msg.version.bytes);
        read(msg.services.bytes);
        read(msg.timestamp.bytes);
        read(msg.recipient_address.services.bytes);
        read(msg.recipient_address.address);
        read(msg.recipient_address.port.bytes);
        read(msg.sender_address.services.bytes);
        read(msg.sender_address.address);
        read(msg.sender_address.port.bytes);
        read(msg.nonce.bytes);
        read(msg.user_agent.length);
        msg.user_agent.string.resize(msg.user_agent.length);
        read(msg.user_agent.string);
        read(msg.start_height.bytes);
        Ensures(msg.user_agent.length == msg.user_agent.string.size());
        return *this;
    }

    auto& operator << (const message::ping& msg)
    {
        write(msg.nonce.bytes);
        return *this;
    }

    auto& operator >> (message::ping& msg)
    {
        read(msg.nonce.bytes);
        return *this;
    }

    auto& operator << (const message::pong& msg)
    {
        write(msg.nonce.bytes);
        return *this;
    }

    auto& operator >> (message::pong& msg)
    {
        read(msg.nonce.bytes);
        return *this;
    }

    auto& operator << (const message::tx& tx)
    {
        Expects(tx.input_count == tx.inputs.size());
        Expects(tx.output_count == tx.outputs.size());
        write(tx.version.bytes);
        write(tx.input_count);
        for(const auto& input : tx.inputs)
        {
            Expects(input.script_length == input.signature_script.size());
            write(input.previous_output.txid.bytes);
            write(input.previous_output.output_index.bytes);
            write(input.script_length);
            write(input.signature_script);
            write(input.sequence.bytes);
        }
        write(tx.output_count);
        for(const auto& output : tx.outputs)
        {
            Expects(output.script_length == output.pubkey_script.size());
            write(output.value.bytes);
            write(output.script_length);
            write(output.pubkey_script);
        }
        write(tx.lock_time.bytes);
        return *this;
    }

    auto& operator >> (message::tx& tx)
    {
        read(tx.version.bytes);
        read(tx.input_count);
        tx.inputs.resize(tx.input_count);
        for(auto& input : tx.inputs)
        {
            read(input.previous_output.txid.bytes);
            read(input.previous_output.output_index.bytes);
            read(input.script_length);
            input.signature_script.resize(input.script_length);
            read(input.signature_script);
            read(input.sequence.bytes);
            Ensures(input.script_length == input.signature_script.size());
        }
        read(tx.output_count);
        tx.outputs.resize(tx.output_count);
        for(auto& output : tx.outputs)
        {
            read(output.value.bytes);
            read(output.script_length);
            output.pubkey_script.resize(output.script_length);
            read(output.pubkey_script);
            Ensures(output.script_length == output.pubkey_script.size());
        }
        read(tx.lock_time.bytes);
        Ensures(tx.input_count == tx.inputs.size());
        Ensures(tx.output_count == tx.outputs.size());
        return *this;
    }

    auto& operator << (const message::block& block)
    {
        Expects(block.transaction_count == block.transactions.size());
        write(block.header.version.bytes);
        write(block.header.previous_block.bytes);
        write(block.header.merkle_root.bytes);
        write(block.header.timestamp.bytes);
        write(block.header.bits.bytes);
        write(block.header.nonce.bytes);
        write(block.transaction_count);
        for(const auto& tx : block.transactions)
            *this << tx;
        return *this;
    }

    auto& operator >> (message::block& block)
    {
        read(block.header.version.bytes);
        read(block.header.previous_block.bytes);
        read(block.header.merkle_root.bytes);
        read(block.header.timestamp.bytes);
        read(block.header.bits.bytes);
        read(block.header.nonce.bytes);
        read(block.transaction_count);
        block.transactions.resize(block.transaction_count);
        for(auto& tx : block.transactions)
            *this >> tx;
        Ensures(block.transaction_count == block.transactions.size());
        return *this;
    }

    void dump(std::ostream& os) const
    {
        for(const auto b : m_buffer)
            os.put(static_cast<char>(b));
    }

private:

    void write(const variable_length_integer& variable)
    {
        write(variable.byte);
        switch(variable.uint8)
        {
            case 0xFDu:
            write(variable.bytes, sizeof variable.uint16);
            break;
            case 0xFEu:
            write(variable.bytes, sizeof variable.uint32);
            break;
            case 0xFFu:
            write(variable.bytes, sizeof variable.uint64);
            break;
        }
    }

    void read(variable_length_integer& variable)
    {
        read(variable.byte);
        switch(variable.uint8)
        {
            case 0xFDu:
            read(variable.bytes, sizeof variable.uint16);
            break;
            case 0xFEu:
            read(variable.bytes, sizeof variable.uint32);
            break;
            case 0xFFu:
            read(variable.bytes, sizeof variable.uint64);
            break;
        }
    }

    void read(byte& b)
    {
        b = *m_current++;
    }

    template<std::size_t N>
    void read(byte (&bytes)[N], const std::size_t size = N)
    {
        std::copy_n(m_current, size, bytes);
    }

    void read(vector<byte>& bytes)
    {
        std::copy_n(m_current, bytes.size(), bytes.begin());
    }

    void write(byte b)
    {
        m_buffer.push_back(b);
    }

    template<std::size_t N>
    void write(const byte (&bytes)[N], const std::size_t size = N)
    {
        std::copy(bytes, bytes + size, std::back_inserter(m_buffer));
    }

    void write(const vector<byte>& bytes)
    {
        std::copy(bytes.begin(), bytes.end(), std::back_inserter(m_buffer));
    }

    vector<byte> m_buffer;

    vector<byte>::const_iterator m_current;
};

} // namespace bitcoin::p2p::message

namespace std {

    inline auto& operator << (std::ostream& os, const bitcoin::p2p::message::payload pl)
    {
        pl.dump(os);
        return os;
    }

} // namespace std
