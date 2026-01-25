#pragma once

#include <common.hpp>
#include <memory>
#include <optional>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

class Type;
class TypeStore;
struct MemberVariable;
struct MemberMethod;
struct Constructor;
struct Destructor;
class Instance;

struct Destructor
{
    virtual ~Destructor() = default;
    virtual void invoke(void* mem) const = 0;
};

class TypeStore
{
public:
    TypeStore() = default;
    ~TypeStore() = default;

    NON_COPYABLE(TypeStore);

    template <typename T>
    Type* get();

private:
    std::unordered_map<std::type_index, std::unique_ptr<Type>> type_map;
};

class Type
{
public:
    template <typename T>
    NO_DISCARD bool is() const;

    NON_COPYABLE(Type);
    DEFAULT_MOVABLE(Type); // todo: this struct should not be movable either

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
    std::vector<MemberVariable> variables {};
    std::vector<std::unique_ptr<MemberMethod>> methods {};
    std::vector<std::unique_ptr<Constructor>> constructor {};

    friend class TypeStore;
};

struct ParameterList
{
    std::vector<Type*> types;
};

struct ArgumentList
{
    std::vector<Instance*> values;
};

struct MemberVariable
{
    std::string name;
    size_t offset {};
    Type* type {};
};

struct MemberMethod
{
    std::string name {};
    ParameterList parameters {};

    MemberMethod() = default;
    virtual ~MemberMethod() = default;
    NON_COPYABLE(MemberMethod);
    NON_MOVABLE(MemberMethod);

    NO_DISCARD virtual Instance invoke(TypeStore& type_store, Instance& object, const ArgumentList& parameters) const = 0;
};

struct Constructor
{
    ParameterList parameters {};

    Constructor() = default;
    virtual ~Constructor() = default;
    NON_COPYABLE(Constructor);
    NON_MOVABLE(Constructor);

    NO_DISCARD virtual Instance invoke(TypeStore& type_store, const ArgumentList& parameters) const = 0;
};

class Instance
{
public:
    Instance() = default;

    /// Instance from raw parts (unsafe)
    Instance(void* ptr, const Type* type)
        : ptr(ptr)
        , type(type) { };

    template <typename T>
    Instance(TypeStore& type_store, T val);

    NON_COPYABLE(Instance);
    Instance(Instance&&) noexcept;
    Instance& operator=(Instance&&) noexcept;
    ~Instance();

    template <typename T>
    const T* cast() const;

    template <typename T>
    T* cast();

    NO_DISCARD bool isValid() const;
    NO_DISCARD bool isType(const Type& type) const;

private:
    void* ptr {};
    const Type* type {};
};
