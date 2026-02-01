#pragma once
#include <type_traits>

namespace reflect::detail
{
template <typename T> using BareType = std::remove_cvref_t<T>;
}