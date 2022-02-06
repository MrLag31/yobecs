#pragma once

#include <compare>
#include <concepts>

namespace yobtk::ecs {

/**
 * \brief This is a simplify "NamedType" with a default operator <=>.
 * 
 * \param Handle The subtype used in these objects. Must have an operator <=>.
 */
template <typename Handle>
requires std::three_way_comparable<Handle> && std::default_initializable<Handle>
class WrappedHandle
{
public:
    /**
     * \brief Default contructor
     */
    WrappedHandle() = default;

    /**
     * \brief Constructs the object with internal value h.
     * 
     * \param h The internal value.
     */
    explicit WrappedHandle(Handle h)
    : h_ { h } {}

    auto operator<=>(const WrappedHandle&) const = default;
    auto& operator*() { return h_; }

private:
    Handle h_;
};

}