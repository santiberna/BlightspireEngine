#pragma once
#include "common.hpp"

#include <compare>
#include <ratio>

namespace bb
{

template <typename T, typename R>
struct Duration
{
    using Ratio = R;
    using Rep = T;
    T value {};

    constexpr Duration operator+(const Duration& rhs) const
    {
        return { value + rhs.value };
    }

    constexpr Duration& operator+=(const Duration& rhs)
    {
        value += rhs.value;
        return *this;
    }

    constexpr Duration operator-(const Duration& rhs) const
    {
        return { value - rhs.value };
    }

    constexpr Duration& operator-=(const Duration& rhs)
    {
        value += rhs.value;
        return *this;
    }

    constexpr std::partial_ordering operator<=>(const Duration&) const = default;
};

template <typename D, typename T, typename R>
constexpr D durationCast(const Duration<T, R>& duration)
{
    using Ratio = std::ratio_divide<R, typename D::Ratio>;
    using Target = typename D::Rep;
    using Wide = std::common_type_t<Target, T, intmax_t>;
    return { static_cast<Target>(static_cast<Wide>(duration.value) * static_cast<Wide>(Ratio::num) / static_cast<Wide>(Ratio::den)) };
}

using MillisecondsF32 = Duration<float, std::milli>;
using NanosecondsI64 = Duration<bb::i64, std::nano>;

class Stopwatch
{
public:
    Stopwatch();
    [[nodiscard]] NanosecondsI64 getElapsed() const;

private:
    NanosecondsI64 nano_start;
};

}