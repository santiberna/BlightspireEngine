#pragma once

#include "common.hpp"

namespace bb
{

template <typename T>
class SlotMap;

template <typename T>
class SlotMapIterator;

template <typename T>
class SlotMapConstIterator;

template <typename T>
class SlotHandle
{
public:
    bool operator==(const SlotHandle&) const = default;
    explicit operator bool() const { return isValid(); }

    [[nodiscard]] bool isValid() const { return version != 0; }
    [[nodiscard]] bb::u32 getIndex() const { return index; }

private:
    bb::u32 index {};
    bb::u32 version {};

    friend SlotMap<T>;
    friend SlotMapIterator<T>;
    friend SlotMapConstIterator<T>;
};

static_assert(sizeof(SlotHandle<struct Dummy>) == 8);

}