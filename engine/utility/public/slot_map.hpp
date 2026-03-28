#pragma once
#include "common.hpp"

#include <optional>
#include <vector>

namespace bb
{

template <typename T>
class SlotMap;

template <typename T>
class SlotMapIterator;

template <typename T>
class SlotMapConstIterator;

class SlotMapKey
{
public:
    bool operator==(const SlotMapKey&) const = default;
    explicit operator bool() const { return version != 0; }

private:
    size_t index {};
    size_t version {};

    template <typename T>
    friend class SlotMap;
    template <typename T>
    friend class SlotMapIterator;
    template <typename T>
    friend class SlotMapConstIterator;
};

template <typename T>
class SlotMapEntry
{
    std::optional<T> value {};
    size_t version = 1;
    friend class SlotMap<T>;
    friend class SlotMapIterator<T>;
    friend class SlotMapConstIterator<T>;
};

template <typename T>
class SlotMapIterator
{
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = std::pair<SlotMapKey, T&>;

    value_type operator*() const
    {
        auto& entry = storage->at(index);
        SlotMapKey key { index, entry.version };
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

    SlotMapIterator(std::vector<SlotMapEntry<T>>* storage, size_t index)
        : storage(storage)
        , index(index)
    {
        skipEmpty();
    }

    friend class SlotMap<T>;
    std::vector<SlotMapEntry<T>>* storage;
    size_t index {};
};

template <typename T>
class SlotMapConstIterator
{
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = std::pair<SlotMapKey, const T&>;

    value_type operator*() const
    {
        auto& entry = storage->at(index);
        SlotMapKey key { index, entry.version };
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

    SlotMapConstIterator(const std::vector<SlotMapEntry<T>>* storage, size_t index)
        : storage(storage)
        , index(index)
    {
        skipEmpty();
    }

    friend class SlotMap<T>;
    const std::vector<SlotMapEntry<T>>* storage;
    size_t index {};
};
template <typename T>
class SlotMap
{
public:
    SlotMap() = default;
    NON_COPYABLE(SlotMap);
    DEFAULT_MOVABLE(SlotMap);

    SlotMapKey insert(T value)
    {
        if (free_list.empty())
        {
            auto& back = storage.emplace_back();
            back.value = std::move(value);
            return SlotMapKey { storage.size() - 1, back.version };
        }
        else
        {
            size_t index = free_list.back();
            free_list.pop_back();

            auto& entry = storage.at(index);
            entry.value = std::move(value);
            return SlotMapKey { index, entry.version };
        }
    }

    void remove(const SlotMapKey& key)
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

    size_t size() const
    {
        return storage.size() - free_list.size();
    }

    T* get(const SlotMapKey& key)
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

    const T* get(const SlotMapKey& key) const
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
    std::vector<size_t> free_list {};
};

}