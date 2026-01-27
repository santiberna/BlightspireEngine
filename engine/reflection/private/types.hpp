#pragma once

#include <common.hpp>
#include <memory>
#include <optional>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

class Type;
class Field;
class Method;
class Constructor;
class Destructor;
class Instance;
class Constant;
class TypeStore
{
public:
    TypeStore() = default;
    ~TypeStore() = default;

    NON_COPYABLE(TypeStore);

    template <typename T> Type* get();

private:
    std::unordered_map<std::type_index, std::unique_ptr<Type>> type_map;
};

class Type
{
public:
    template <typename T> NO_DISCARD bool is() const;

    NON_COPYABLE(Type);
    DEFAULT_MOVABLE(Type);

    NO_DISCARD std::optional<uint64_t> getConstant(std::string_view name) const;

    NO_DISCARD const Destructor* getDestructor() const;
    NO_DISCARD std::type_index getIndex() const;

private:
    Type() = default;

    // Always initialized
    std::string name {};
    size_t size {};
    const type_info* index {};
    std::unique_ptr<Destructor> destructor {};

    // Factory set
    std::optional<std::string> alias {};
    std::vector<Field> fields {};
    std::vector<std::unique_ptr<Method>> methods {};
    std::vector<std::unique_ptr<Constructor>> constructor {};
    std::unordered_map<std::string, uint64_t> constants {};

    friend TypeStore;

    template <typename T> friend class TypeBuilder;
};

struct ArgumentList
{
    std::vector<Instance*> values;
};

class Instance
{
public:
    Instance() = default;

    /// Instance from raw parts (unsafe)
    Instance(void* ptr, const Type* type)
        : ptr(ptr)
        , type(type) { };

    template <typename T> Instance(TypeStore& type_store, T val);

    NON_COPYABLE(Instance);
    Instance(Instance&&) noexcept;
    Instance& operator=(Instance&&) noexcept;
    ~Instance();

    template <typename T> const T* cast() const;

    template <typename T> T* cast();

    NO_DISCARD bool isValid() const;
    NO_DISCARD bool isType(const Type& type) const;
    NO_DISCARD const void* getAddress() const;

private:
    void* ptr {};
    const Type* type {};
};

struct ParameterList
{
    std::vector<Type*> types;

    NO_DISCARD bool validateArgs(const ArgumentList& args) const
    {
        if (types.size() != args.values.size())
        {
            return false;
        }
        for (size_t i = 0; i < types.size(); i++)
        {
            auto* arg = args.values[i];
            auto* type = types[i];
            if (!arg->isType(*type))
            {
                return false;
            }
        }
        return true;
    }
};