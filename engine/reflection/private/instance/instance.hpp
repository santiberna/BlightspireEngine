#pragma once
#include <common.hpp>

class Type;

class Instance
{
public:
    Instance() = default;

    /// Instance from raw parts (unsafe)
    Instance(void* ptr, const Type* type)
        : ptr(ptr)
        , type(type) { };

    template <typename T> Instance(const Type* type_store, T val);

    NON_COPYABLE(Instance);
    Instance(Instance&&) noexcept;
    Instance& operator=(Instance&&) noexcept;
    ~Instance();

    template <typename T> const T* cast() const;

    template <typename T> T* cast();

    NO_DISCARD bool isValid() const;
    NO_DISCARD const Type* getType() const;
    NO_DISCARD const void* getAddress() const;

private:
    void* ptr {};
    const Type* type {};
};

#include <instance/instance_impl.hpp>