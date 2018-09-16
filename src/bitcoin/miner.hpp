#pragma once
#include "bitcoin/primitives.hpp"
#include "bitcoin/message/payload.hpp"
#include "cryptic/sha2.hpp"

namespace bitcoin
{

void mine()
{
    auto block = bitcoin::block{};
    auto payload = bitcoin::message::payload{};
    auto hash = cryptic::sha256{};

    payload << block;
    hash.update(payload.as_bytes().first<64>());

    auto tail = payload.as_bytes().last<16>();

    for(auto h = 0u; h < 256u; ++h)
    {
        tail[15] = static_cast<bitcoin::byte>(h);

        for(auto i = 0u; i < 256u; ++i)
        {
            tail[14] = static_cast<bitcoin::byte>(i);

            for(auto j = 0u; j < 256u; ++j)
            {
                tail[13] = static_cast<bitcoin::byte>(j);

                for(auto k = 0u; k < 256u; ++k)
                {
                    tail[12] = static_cast<bitcoin::byte>(k);

                    auto test = hash;
                    test.finalize(tail);

                    if(test.hexadecimal().substr(0,7) == "0000000")
                    {
                        std::clog << block.header.nonce << std::endl;
                        std::clog << test.hexadecimal() << std::endl;
                        return;
                    }
                }
            }
        }
    }
}

} // namespace bitcoin
