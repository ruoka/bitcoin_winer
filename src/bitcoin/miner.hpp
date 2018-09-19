#pragma once
#include <thread>
#include <cstdint>
#include "bitcoin/message/payload.hpp"
#include "cryptic/sha2.hpp"

// 0000000000000000001bf6a7d2d86ba07971f343cee7e9cea305bdc1a7dfe4d6

// https://en.bitcoin.it/wiki/Transaction_fees

// http://bitcoinfees.com

namespace bitcoin
{

void mine(const bitcoin::block block, std::uint_fast16_t begin = 0u, std::uint_fast16_t end = 256u)
{
    static auto s_stop = std::atomic_bool{false};
    static auto s_mutex = std::mutex{};
    {
        const auto lock = std::scoped_lock{s_mutex};
        std::clog << "Begin of " << begin << " - " << block.header.timestamp << std::endl;
    }

    auto hash = cryptic::sha256{};

    auto payload = bitcoin::message::payload{};

    payload << block;

    Expects(payload.length() >= 80u);

    auto head = payload.as_bytes().first<64>();

    auto tail = payload.as_bytes().subspan(64,16);

    auto nonce = payload.as_bytes().subspan(76,4);

    hash.update(head);

    const auto target = block.target();

    // for(auto h = begin; h < end; ++h)
    for(auto h = std::uint_fast16_t{0u}; h < 256u; ++h)
    {
        nonce[3] = static_cast<bitcoin::byte>(h);

        for(auto i = begin; i < end; ++i)
        // for(auto i = std::uint_fast16_t{0u}; i < 256u; ++i)
        {
            nonce[2] = static_cast<bitcoin::byte>(i);

            // for(auto j = begin; j < end; ++j)
            for(auto j = std::uint_fast16_t{0u}; j < 256u; ++j)
            {
                nonce[1] = static_cast<bitcoin::byte>(j);

                // for(auto k = begin; k < end; ++k)
                for(auto k = std::uint_fast16_t{0u}; k < 256u; ++k)
                {
                    nonce[0] = static_cast<bitcoin::byte>(k);

                    auto test = hash;

                    test.finalize(tail);

                    if(s_stop) goto end;

                    if(test < target.as_bytes())
                    {
                        s_stop = true;
                        const auto lock = std::scoped_lock{s_mutex};
                        std::clog << "Nonce found " << begin << " - " << block.header.timestamp << std::endl;
                        std::clog << *reinterpret_cast<std::uint64_t*>(nonce.data()) << std::endl;
                        std::clog << test.hexadecimal() << std::endl;
                        goto end;
                    }
                }
                std::this_thread::yield();
            }
        }
    }
    {
        const auto lock = std::scoped_lock{s_mutex};
        std::clog << "No results " << begin << " - " << block.header.timestamp << std::endl;
    }
end:
    {
        const auto lock = std::scoped_lock{s_mutex};
        std::clog << "End of " << begin << " - " << block.header.timestamp << std::endl;
    }
}

void pool_mine(bitcoin::block& block)
{
    auto threads = std::vector<std::thread>{};
    for(auto i = 0; i < 10; ++i)
    {
        block.header.timestamp = block.header.timestamp + 1;
        threads.push_back(std::thread{[&](){mine(block,   0u,  32u);}});
        threads.push_back(std::thread{[&](){mine(block,  32u,  64u);}});
        threads.push_back(std::thread{[&](){mine(block,  64u,  96u);}});
        threads.push_back(std::thread{[&](){mine(block,  96u, 128u);}});
        threads.push_back(std::thread{[&](){mine(block, 128u, 160u);}});
        threads.push_back(std::thread{[&](){mine(block, 160u, 192u);}});
        threads.push_back(std::thread{[&](){mine(block, 192u, 224u);}});
        threads.push_back(std::thread{[&](){mine(block, 224u, 266u);}});
        std::this_thread::sleep_for(100ms);
    }

    for(auto& thread : threads)
        thread.join();
}

} // namespace bitcoin
