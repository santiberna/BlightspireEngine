#pragma once

constexpr bb::u64 operator""_kb(unsigned long long kilobytes)
{
    return kilobytes * 1024;
}

constexpr bb::u64 operator""_mb(unsigned long long megabytes)
{
    return megabytes * 1024 * 1024;
}

constexpr bb::u64 operator""_gb(unsigned long long gigabytes)
{
    return gigabytes * 1024 * 1024 * 1024;
}