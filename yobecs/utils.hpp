#pragma once

#include <cstdint>
#include <type_traits>

namespace yobtk::utils {

// indexVariadicTypePack
// Based upon:
// https://stackoverflow.com/questions/26169198/how-to-get-the-index-of-a-type-in-a-variadic-type-pack

using _indexVtp_t = std::size_t;

template <typename T, typename ... Ts>
struct _indexVtp;

template <typename T, typename ... Ts>
static constexpr auto _indexVtp_v = _indexVtp<T, Ts ...>::value;

template <typename T, typename ... Ts>
struct _indexVtp<T, T, Ts ...>
: public std::integral_constant<_indexVtp_t, 0> {};

template <typename T, typename U, typename ... Ts>
struct _indexVtp<T, U, Ts ...>
: public std::integral_constant<_indexVtp_t, 1 + _indexVtp_v<T, Ts ...>> {};

template <typename T, typename ... Ts>
static constexpr auto indexVariadicTypePack = _indexVtp_v<T, Ts ...>;

}