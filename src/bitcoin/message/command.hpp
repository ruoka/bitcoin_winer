#pragma once
#include <array>
#include "gsl/span.hpp"
#include "bitcoin/message/messages.hpp"

using namespace std::literals;

namespace bitcoin::message
{

class command
{
public:
    command() = default;
    command(const version&) : bytes{'v','e','r','s','i','o','n'}
    {}
    command(const verack&) : bytes{'v','e','r','a','c','k'}
    {}
    command(const ping&) : bytes{'p','i','n','g'}
    {}
    command(const pong&) : bytes{'p','o','n','g'}
    {}
    command(const tx&) : bytes{'t','x'}
    {}
    command(const block&) : bytes{'b','l','o','c','k'}
    {}
    bool is_version() const
    {
        return gsl::make_span(bytes).first<8>() == gsl::make_span("version");
    }
    bool is_verack() const
    {
        return gsl::make_span(bytes).first<7>() == gsl::make_span("verack");
    }
    bool is_ping() const
    {
        return gsl::make_span(bytes).first<5>() == gsl::make_span("ping");
    }
    bool is_pong() const
    {
        return gsl::make_span(bytes).first<5>() == gsl::make_span("pong");
    }
    bool is_tx() const
    {
        return gsl::make_span(bytes).first<3>() == gsl::make_span("tx");
    }
    bool is_block() const
    {
        return gsl::make_span(bytes).first<6>() == gsl::make_span("block");
    }
    auto as_bytes()
    {
        return gsl::as_writeable_bytes<char>(gsl::make_span(bytes));
    }
    auto as_bytes() const
    {
        return gsl::as_bytes<const char>(gsl::make_span(bytes));
    }
private:
    std::array<char,12> bytes = {'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
};

} // namespace bitcoin::message
