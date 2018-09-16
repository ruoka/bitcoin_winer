#pragma once
#include "bitcoin/primitives.hpp"
#include "bitcoin/message/payload.hpp"
#include "cryptic/sha2.hpp"

// 0000000000000000001bf6a7d2d86ba07971f343cee7e9cea305bdc1a7dfe4d6

namespace bitcoin
{

void mine()
{
    auto block = bitcoin::block{};
    auto payload = bitcoin::message::payload{};
    auto hash = cryptic::sha256{};

    hash.encode(block.header.previous_block.as_bytes());
    hash.encode(block.header.merkle_root.as_bytes());
    block.header.timestamp = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    block.header.bits = 0x1d00fffful;
    // block.header.bits = 0x172819a1;
    // block.header.bits = 0x1e00fffful;

    payload << block;

    Expects(payload.size() == 80);

    auto head = payload.as_bytes().first<64>();

    auto tail = payload.as_bytes().last<16>();

    auto nonce = payload.as_bytes().last<4>();

    hash.update(head);

    auto target = block.target();

    for(auto h = 0u; h < 256u; ++h)
    {
        nonce[3] = static_cast<bitcoin::byte>(h);

        for(auto i = 0u; i < 256u; ++i)
        {
            nonce[2] = static_cast<bitcoin::byte>(i);

            for(auto j = 0u; j < 256u; ++j)
            {
                nonce[1] = static_cast<bitcoin::byte>(j);

                for(auto k = 0u; k < 256u; ++k)
                {
                    nonce[0] = static_cast<bitcoin::byte>(k);

                    auto test = hash;

                    test.finalize(tail);

                    if(test < target.as_bytes())
                    {
                        std::clog << *reinterpret_cast<std::uint64_t*>(nonce.data()) << std::endl;
                        std::clog << test.hexadecimal() << std::endl;
                        return;
                    }
                }
            }
        }
    }
}

} // namespace bitcoin
