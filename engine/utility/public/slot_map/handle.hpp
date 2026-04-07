#pragma once

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
    explicit operator bool() const { return version != 0; }

private:
    size_t index {};
    size_t version {};

    friend SlotMap<T>;
    friend SlotMapIterator<T>;
    friend SlotMapConstIterator<T>;
};

}