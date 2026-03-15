#include <member/method.hpp>
#include <stdexcept>
#include <type/type.hpp>
#include <utility/argument_list.hpp>
#include <value/value.hpp>
#include <value/value_ref.hpp>

using namespace reflection;

const Type* ValueRef::getType() const { return this->type; }

ValueRef::ValueRef(void* value, const Type* type)
    : value(value)
    , type(type)
{
}

[[nodiscard]] Value ValueRef::call(std::string_view name, const ArgumentList& args)
{
    if (const auto* method = type->getMethod(name))
    {
        return method->invoke(*this, args);
    }
    throw std::runtime_error("No matching method name found");
}

ValueRef ValueRef::access(std::string_view name) const
{
    if (const auto* field = type->getField(name))
    {
        auto* ptr = field->rawAccess(value);
        return { ptr, field->getType() };
    }
    throw std::runtime_error("No matching field name found");
}