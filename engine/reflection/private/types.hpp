#pragma once

#include <common.hpp>
#include <memory>
#include <optional>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include <utility/argument_list.hpp>
#include <utility/parameter_list.hpp>

class Field;
class Method;
class Constructor;
class Destructor;
class Constant;
class Instance;

class TypeStore
{
public:
    TypeStore() = default;
    ~TypeStore() = default;

    NON_COPYABLE(TypeStore);

    template <typename T> NO_DISCARD Type* get();
    template <typename T> NO_DISCARD Instance makeInstance(T&& val);

private:
    std::unordered_map<std::type_index, std::unique_ptr<Type>> type_map;
};

class Type
{
public:
    template <typename T> NO_DISCARD bool is() const;

    NON_COPYABLE(Type);
    DEFAULT_MOVABLE(Type);

    NO_DISCARD Instance construct(const ArgumentList& args) const;
    NO_DISCARD std::optional<uint64_t> getConstant(std::string_view name) const;
    NO_DISCARD const Destructor* getDestructor() const;
    NO_DISCARD std::type_index getIndex() const;

private:
    Type(TypeStore* owner_store);

    // Always initialized
    TypeStore* owner_store {};

    std::string name {};
    size_t size {};
    const type_info* index {};
    std::unique_ptr<Destructor> destructor {};

    // Factory set
    std::optional<std::string> alias {};
    std::vector<Field> fields {};
    std::vector<std::unique_ptr<Method>> methods {};

    std::unordered_map<ParameterList, std::unique_ptr<Constructor>> constructors {};
    std::unordered_map<std::string, uint64_t> constants {};

    friend TypeStore;
    template <typename T> friend class TypeBuilder;
};

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