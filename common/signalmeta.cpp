#include "signalmeta.h"

#include <iostream>
#include <sstream>


namespace microsynth
{
    namespace
    {
        // Private helpers

        template <typename Head0, typename Head1, typename... Tail>
        constexpr auto max(const Head0& head0, const Head1& head1, const Tail&... tail)
        {
            if constexpr (sizeof...(tail) == 0)
            {
                return head0 > head1 ? head0 : head1;
            }
            else
            {
                return max(max(head0, head1), tail...);
            }
        }
    }

    std::unique_ptr<queueable> queueable::copy() const
    {
        return std::make_unique<queueable>(queueable{
            .buf = this->buf,
            .length = this->length,
            .capabilities = this->capabilities // copy
            // (discards the runtime state, though...)
        });
    }

    // template <typename... Args>
    // std::unique_ptr<queueable> mix(Args... pack)
    // {
    //     constexpr bool ok = std::conjunction_v<std::is_assignable<Args, queueable>...>;
    //     static_assert(ok, "Arguments to mix(...) need to be convertable to 'microsynth::queueable'");
    //     const size_t length = max(pack->length...);
    //     std::cout << "length: " << length << "\n";
    //     throw std::runtime_error("a");
    //     return std::make_unique<queueable>(queueable {
    //     });
    // }

    void reid(queueable& target)
    {
        target.id = _next_ID++;
    }

    std::ostream& operator<<(std::ostream& os, queueable const& m)
    {
        std::stringstream collect {};
        collect << "{" << m.id << "}";
        return os << collect.str();
    }
}
