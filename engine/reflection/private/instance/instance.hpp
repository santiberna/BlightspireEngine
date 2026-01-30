#pragma once
#include <common.hpp>

#include <memory>
#include <string_view>

class Type;

class VoidInstanceType
{
};
constexpr VoidInstanceType VOID_INSTANCE = {};

class Instance
{
public:
    Instance(VoidInstanceType);
    bool operator==(VoidInstanceType) const;

    template <typename T> NO_DISCARD bool is() const;
    template <typename T> NO_DISCARD std::shared_ptr<const T> cast() const;
    template <typename T> NO_DISCARD std::shared_ptr<T> cast();
    /// TODO: add call operators here

    NO_DISCARD Instance access(std::string_view name) const;
    NO_DISCARD const Type* getType() const;

private:
    friend class TypeStore; // For access to private constructor
    Instance(std::shared_ptr<void> value, const Type* type);

    std::shared_ptr<void> value {};
    const Type* type {};
};

#include <instance/instance_impl.hpp>