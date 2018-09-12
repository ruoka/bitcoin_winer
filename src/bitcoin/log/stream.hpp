#include "bitcoin/primitives.hpp"

namespace bitcoin::log
{

struct stream : public std::ostream
{
    stream(std::ostream& os) : parent{os}
    {
        init(os.rdbuf());
    }

    auto& operator << (const unsigned_integer& type)
    {
        parent << std::hex << type;
        return *this;
    }

    auto& operator<< (stream& (*pf)(stream&))
    {
        (*pf)(*this);
        return *this;
    }

    std::ostream& parent;
};

inline auto& flush(stream& os)
{
    os.put('\n').flush();
    return os;
}

}
