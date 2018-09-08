namespace bitcoin::p2p
{

struct stream : public std::iostream
{
    stream(std::ostream& os) : std::iostream{os.rdbuf()}
    {
        init(os.rdbuf());
    }

    auto& operator << (const message::header& header)
    {
        write(reinterpret_cast<const char_type*>(header.magic.bytes), sizeof header.magic.bytes);
        write(reinterpret_cast<const char_type*>(header.command), sizeof header.command);
        write(reinterpret_cast<const char_type*>(header.payload_length.bytes), sizeof header.payload_length.bytes);
        write(reinterpret_cast<const char_type*>(header.checksum.bytes), sizeof header.checksum.bytes);
        return *this;
    }

    auto& operator >> (message::header& header)
    {
        read(reinterpret_cast<char_type*>(header.magic.bytes), sizeof header.magic.bytes);
        read(reinterpret_cast<char_type*>(header.command), sizeof header.command);
        read(reinterpret_cast<char_type*>(header.payload_length.bytes), sizeof header.payload_length.bytes);
        read(reinterpret_cast<char_type*>(header.checksum.bytes), sizeof header.checksum.bytes);
        return *this;
    }

    auto& operator << (const decltype(block::header)& header)
    {
        write(reinterpret_cast<const char_type*>(header.version.bytes), sizeof header.version.bytes);
        write(reinterpret_cast<const char_type*>(header.previous_block.bytes), sizeof header.previous_block.bytes);
        write(reinterpret_cast<const char_type*>(header.merkle_root.bytes), sizeof header.merkle_root.bytes);
        write(reinterpret_cast<const char_type*>(header.timestamp.bytes), sizeof header.timestamp.bytes);
        write(reinterpret_cast<const char_type*>(header.bits.bytes), sizeof header.bits.bytes);
        write(reinterpret_cast<const char_type*>(header.nonce.bytes), sizeof header.nonce.bytes);
        return *this;
    }

    auto& operator >> (decltype(block::header)& header)
    {
        read(reinterpret_cast<char_type*>(header.version.bytes), sizeof header.version.bytes);
        read(reinterpret_cast<char_type*>(header.previous_block.bytes), sizeof header.previous_block.bytes);
        read(reinterpret_cast<char_type*>(header.merkle_root.bytes), sizeof header.merkle_root.bytes);
        read(reinterpret_cast<char_type*>(header.timestamp.bytes), sizeof header.timestamp.bytes);
        read(reinterpret_cast<char_type*>(header.bits.bytes), sizeof header.bits.bytes);
        read(reinterpret_cast<char_type*>(header.nonce.bytes), sizeof header.nonce.bytes);
        return *this;
    }

    auto& operator << (const p2p::message::tx& tx)
    {
        return *this;
    }

    auto& operator >> (p2p::message::tx& tx)
    {
        return *this;
    }

    auto& operator<< (stream& (*pf)(stream&))
    {
        (*pf)(*this);
        return *this;
    }
};

inline auto& flush(stream& os)
{
    os.flush();
    return os;
}

}
