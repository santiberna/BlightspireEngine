#pragma once

#include <cstdint>

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
    SlotHandle() = default;
    bool operator==(const SlotHandle&) const = default;
    explicit operator bool() const { return isValid(); }

    [[nodiscard]] uint32_t getIndex() const { return index; }
    [[nodiscard]] bool isValid() const { return version != 0; }

private:
    uint32_t index {};
    uint32_t version {};

    friend SlotMap<T>;
    friend SlotMapIterator<T>;
    friend SlotMapConstIterator<T>;
};

static_assert(sizeof(SlotHandle<struct Dummy>) == 8);

}