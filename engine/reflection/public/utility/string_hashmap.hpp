#pragma once
#include <string>
#include <unordered_map>

namespace reflection::detail
{

struct HeterogenousStringHash
{
    using is_transparent = void; // NOLINT

    bb::usize operator()(std::string_view sv) const { return std::hash<std::string_view> {}(sv); }
};

struct HeterogenousStringEqual
{
    using is_transparent = void; // NOLINT

    bool operator()(std::string_view lhs, std::string_view rhs) const { return lhs == rhs; }
};

template <typename T>
using StringHashmap
    = std::unordered_map<std::string, T, HeterogenousStringHash, HeterogenousStringEqual>;
}