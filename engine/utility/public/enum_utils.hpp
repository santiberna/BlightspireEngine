#pragma once

#include <initializer_list>
#include <type_traits>

#define GENERATE_ENUM_FLAG_OPERATORS(EnumType)                                                                                                                                                   \
    extern "C++"                                                                                                                                                                                 \
    {                                                                                                                                                                                            \
        inline EnumType operator|(EnumType a, EnumType b) { return static_cast<EnumType>(static_cast<std::underlying_type_t<EnumType>>(a) | static_cast<std::underlying_type_t<EnumType>>(b)); } \
        inline EnumType operator&(EnumType a, EnumType b) { return static_cast<EnumType>(static_cast<std::underlying_type_t<EnumType>>(a) & static_cast<std::underlying_type_t<EnumType>>(b)); } \
        inline EnumType operator~(EnumType a) { return static_cast<EnumType>(~static_cast<std::underlying_type_t<EnumType>>(a)); }                                                               \
        inline EnumType operator^(EnumType a, EnumType b) { return static_cast<EnumType>(static_cast<std::underlying_type_t<EnumType>>(a) ^ static_cast<std::underlying_type_t<EnumType>>(b)); } \
    }

template <typename EnumType>
inline bool HasAnyFlags(EnumType lhs, EnumType rhs)
{
    return static_cast<int>(lhs & rhs) != 0;
}

namespace bb
{

template <typename E>
    requires std::is_enum_v<E>
struct Flags
{
    using Storage = std::underlying_type_t<E>;

    Flags() = default;
    Flags(E flag)
        : value(static_cast<Storage>(flag))
    {
    }
    Flags(std::initializer_list<E> flags)
    {
        for (auto f : flags)
        {
            value |= static_cast<Storage>(f);
        }
    }

    bool has(E flag) const
    {
        return (value & static_cast<Storage>(flag)) != 0;
    }

    Flags& set(E flag)
    {
        value |= static_cast<Storage>(flag);
        return *this;
    }

private:
    Flags(Storage value)
        : value(value)
    {
    }
    Storage value {};
};

}