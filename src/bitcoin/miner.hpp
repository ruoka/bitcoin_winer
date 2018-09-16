#pragma once
#include <thread>
#include <cstdint>
#include "bitcoin/primitives.hpp"
#include "bitcoin/message/payload.hpp"
#include "cryptic/sha2.hpp"

// 0000000000000000001bf6a7d2d86ba07971f343cee7e9cea305bdc1a7dfe4d6

namespace bitcoin
{

void mine(std::chrono::system_clock::time_point timestamp, std::uint_fast16_t begin = 0u, std::uint_fast16_t end = 256u)
{
    static auto s_mutex = std::mutex{};
    {
        const auto lock = std::scoped_lock{s_mutex};
        std::clog << "Begin of " << begin << std::endl;
    }

    auto block = bitcoin::block{};
    auto payload = bitcoin::message::payload{};
    auto hash = cryptic::sha256{};

    hash.encode(block.header.previous_block.as_bytes());
    hash.encode(block.header.merkle_root.as_bytes());
    block.header.timestamp = std::chrono::duration_cast<std::chrono::seconds>(timestamp.time_since_epoch()).count();
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

    for(auto h = std::uint_fast16_t{0u}; h < 256u; ++h)
    {
        nonce[3] = static_cast<bitcoin::byte>(h);

        for(auto i = std::uint_fast16_t{0u}; i < 256u; ++i)
        {
            nonce[2] = static_cast<bitcoin::byte>(i);

            for(auto j = begin; j < end; ++j)
            {
                nonce[1] = static_cast<bitcoin::byte>(j);

                for(auto k = std::uint_fast16_t{0u}; k < 256u; ++k)
                {
                    nonce[0] = static_cast<bitcoin::byte>(k);

                    auto test = hash;

                    test.finalize(tail);

                    if(test < target.as_bytes())
                    {
                        const auto lock = std::scoped_lock{s_mutex};
                        std::clog << "Nonce found " << begin << std::endl;
                        std::clog << *reinterpret_cast<std::uint64_t*>(nonce.data()) << std::endl;
                        std::clog << test.hexadecimal() << std::endl;
                        goto end;
                    }
                }
            }
        }
    }
    {
        const auto lock = std::scoped_lock{s_mutex};
        std::clog << "No results " << begin << std::endl;
    }
end:
    {
        const auto lock = std::scoped_lock{s_mutex};
        std::clog << "End of " << begin << std::endl;
    }
}

void pool_mine(std::chrono::system_clock::time_point timestamp)
{
    auto thread1 = std::thread{[&](){mine(timestamp,   0u,  64u);}};
    auto thread2 = std::thread{[&](){mine(timestamp,  64u, 128u);}};
    auto thread3 = std::thread{[&](){mine(timestamp, 128u, 192u);}};
    auto thread4 = std::thread{[&](){mine(timestamp, 192u, 246u);}};
    std::this_thread::yield();
    thread1.join();
    thread2.join();
    thread3.join();
    thread4.join();
}

} // namespace bitcoin
