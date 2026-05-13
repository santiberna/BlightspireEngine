#pragma once

#include "slot_map/handle.hpp"

#include <optional>
#include <vector>

namespace bb
{

template <typename T>
class SlotMapEntry
{
    std::optional<T> value {};
    bb::usize version = 1;
    friend SlotMap<T>;
    friend SlotMapIterator<T>;
    friend SlotMapConstIterator<T>;
};

template <typename T>
class SlotMapIterator
{
public:
    using iterator_category = std::forward_iterator_tag; // NOLINT
    using value_type = std::pair<SlotHandle<T>, T&>; // NOLINT

    value_type operator*() const
    {
        auto& entry = storage->at(index);
        SlotHandle<T> key;
        key.index = index;
        key.version = entry.version;
        return { key, *entry.value };
    }

    SlotMapIterator& operator++()
    {
        ++index;
        skipEmpty();
        return *this;
    }

    bool operator==(const SlotMapIterator& other) const { return index == other.index; }
    bool operator!=(const SlotMapIterator& other) const { return index != other.index; }

private:
    void skipEmpty()
    {
        while (index < storage->size() && !storage->at(index).value.has_value())
        {
            ++index;
        }
    }

    SlotMapIterator(std::vector<SlotMapEntry<T>>* storage, bb::usize index)
        : storage(storage)
        , index(index)
    {
        skipEmpty();
    }

    friend class SlotMap<T>;
    std::vector<SlotMapEntry<T>>* storage;
    bb::usize index {};
};

template <typename T>
class SlotMapConstIterator
{
public:
    using iterator_category = std::forward_iterator_tag; // NOLINT
    using value_type = std::pair<SlotHandle<T>, const T&>; // NOLINT

    value_type operator*() const
    {
        auto& entry = storage->at(index);
        SlotHandle<T> key;
        key.index = index;
        key.version = entry.version;
        return { key, *entry.value };
    }

    SlotMapConstIterator& operator++()
    {
        ++index;
        skipEmpty();
        return *this;
    }

    bool operator==(const SlotMapConstIterator& other) const { return index == other.index; }
    bool operator!=(const SlotMapConstIterator& other) const { return index != other.index; }

private:
    void skipEmpty()
    {
        while (index < storage->size() && !storage->at(index).value.has_value())
        {
            ++index;
        }
    }

    SlotMapConstIterator(const std::vector<SlotMapEntry<T>>* storage, bb::usize index)
        : storage(storage)
        , index(index)
    {
        skipEmpty();
    }

    friend class SlotMap<T>;
    const std::vector<SlotMapEntry<T>>* storage;
    bb::usize index {};
};
template <typename T>
class SlotMap
{
public:
    SlotMap() = default;
    NON_COPYABLE(SlotMap);
    DEFAULT_MOVABLE(SlotMap);

    SlotHandle<T> insert(T value)
    {
        SlotHandle<T> out {};
        if (free_list.empty())
        {
            auto& back = storage.emplace_back();
            back.value = std::move(value);

            out.index = storage.size() - 1;
            out.version = back.version;
        }
        else
        {
            bb::usize index = free_list.back();
            free_list.pop_back();

            auto& entry = storage.at(index);
            entry.value = std::move(value);

            out.index = index;
            out.version = entry.version;
        }
        return out;
    }

    void remove(const SlotHandle<T>& key)
    {
        if (key.index < storage.size())
        {
            auto& entry = storage.at(key.index);
            if (entry.version == key.version)
            {
                free_list.emplace_back(key.index);
                entry.value.reset();
                entry.version += 1;
            }
        }
    }

    bb::usize storageSize() const
    {
        return storage.size();
    }

    bb::usize storageCount() const
    {
        return storage.size() - free_list.size();
    }

    T* get(const SlotHandle<T>& key)
    {
        if (key.index < storage.size())
        {
            auto& entry = storage.at(key.index);
            if (entry.version == key.version)
            {
                return &entry.value.value();
            }
        }
        return nullptr;
    }

    const T* get(const SlotHandle<T>& key) const
    {
        return const_cast<SlotMap*>(this)->get(key);
    }

    SlotMapIterator<T> begin()
    {
        return { &storage, 0 };
    }

    SlotMapIterator<T> end()
    {
        return { &storage, storage.size() };
    }

    SlotMapConstIterator<T> begin() const
    {
        return { &storage, 0 };
    }

    SlotMapConstIterator<T> end() const
    {
        return { &storage, storage.size() };
    }

private:
    std::vector<SlotMapEntry<T>> storage {};
    std::vector<bb::usize> free_list {};
};

}