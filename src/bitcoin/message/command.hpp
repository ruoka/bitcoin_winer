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
    bool is_version() const
    {
        return gsl::make_span(bytes) == gsl::make_span("version"sv);
    }
    bool is_verack() const
    {
        return gsl::make_span(bytes) == gsl::make_span("verack"sv);
    }
    bool is_ping() const
    {
        return gsl::make_span(bytes) == gsl::make_span("ping"sv);
    }
    bool is_pong() const
    {
        return gsl::make_span(bytes) == gsl::make_span("pong"sv);
    }
    auto as_bytes()
    {
        return gsl::as_writeable_bytes<char>(bytes);
    }
    auto as_bytes() const
    {
        return gsl::as_bytes<const char>(bytes);
    }
private:
    std::array<char,12> bytes = {'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
};

} // namespace bitcoin::message
