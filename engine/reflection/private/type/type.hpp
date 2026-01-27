#pragma once

#include <common.hpp>

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include <utility/argument_list.hpp>
#include <utility/parameter_list.hpp>

class Constructor;
class TypeStore;
class Instance;
class Destructor;
class Field;
class Method;

class Type
{
public:
    template <typename T> NO_DISCARD bool is() const;

    NON_COPYABLE(Type);
    DEFAULT_MOVABLE(Type);
    ~Type();

    NO_DISCARD Instance construct(const ArgumentList& args) const;
    NO_DISCARD std::optional<uint64_t> getConstant(std::string_view name) const;
    NO_DISCARD const Destructor* getDestructor() const;
    NO_DISCARD std::type_index getIndex() const;

private:
    Type();

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

template <typename T> bool Type::is() const { return this->getIndex() == typeid(T); }