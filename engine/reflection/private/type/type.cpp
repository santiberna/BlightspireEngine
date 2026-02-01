#include <type/type.hpp>

#include <cassert>
#include <constructor/constructor.hpp>
#include <member/field.hpp>
#include <member/method.hpp>
#include <utility/argument_list.hpp>
#include <utility/parameter_list.hpp>
#include <value/value.hpp>

using namespace reflect;
using namespace reflect::detail;

Type::Type() = default;
Type::~Type() = default;

std::type_index Type::getIndex() const { return { *this->index }; }

std::optional<uint64_t> Type::getConstant(std::string_view name) const
{
    if (auto it = this->constants.find(std::string(name)); it != this->constants.end())
    {
        return it->second;
    }
    return std::nullopt;
}

const Field* Type::getField(std::string_view name) const
{
    if (auto it = this->fields.find(std::string(name)); it != this->fields.end())
    {
        return it->second.get();
    }
    return nullptr;
}

const Method* Type::getMethod(std::string_view name) const
{
    if (auto it = this->methods.find(std::string(name)); it != this->methods.end())
    {
        return it->second.get();
    }
    return nullptr;
}

Value Type::construct(const ArgumentList& args) const
{
    auto params = args.asTypes();
    auto list = ParameterList(params);

    if (auto it = constructors.find(list); it != constructors.end())
    {
        return it->second->invoke(args);
    }
    throw std::runtime_error("Failed to find matching constructor.");
}