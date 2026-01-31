#include <instance/instance_ref.hpp>

#include <instance/instance.hpp>
#include <member/method.hpp>
#include <stdexcept>
#include <type/type.hpp>

const Type* InstanceRef::getType() const { return this->type; }

InstanceRef::InstanceRef(void* value, const Type* type)
    : value(value)
    , type(type)
{
}

NO_DISCARD Instance InstanceRef::call(std::string_view name, const ArgumentList& args)
{
    if (const auto* method = type->getMethod(name))
    {
        return method->invoke(*this, args);
    }
    throw std::runtime_error("No matching method name found");
}

InstanceRef InstanceRef::access(std::string_view name) const
{
    if (const auto* field = type->getField(name))
    {
        auto* ptr = field->rawAccess(value);
        return { ptr, field->getType() };
    }
    throw std::runtime_error("No matching field name found");
}