#pragma once
#include <type_traits>

namespace reflect::detail
{
template <typename T> using BareType = std::remove_cvref_t<T>;
template <typename T> struct IsReflected : std::false_type
{
};
}

namespace reflect
{
template <typename T>
concept ReflectedType = detail::IsReflected<T>::value;
}