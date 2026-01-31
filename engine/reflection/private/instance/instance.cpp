#include <instance/instance.hpp>
#include <member/method.hpp>

Instance::Instance(MAYBE_UNUSED VoidInstanceType _unused) { }

bool Instance::operator==(MAYBE_UNUSED VoidInstanceType _unused) const { return value == nullptr; }

Instance::Instance(std::shared_ptr<void> value, const Type* type)
    : value(std::move(value))
    , type(type)
{
}

NO_DISCARD Instance Instance::call(std::string_view name, const ArgumentList& args)
{
    if (const auto* method = type->getMethod(name))
    {
        return method->invoke(*this, args);
    }
    throw std::runtime_error("No matching method name found");
}

const Type* Instance::getType() const { return this->type; }

Instance Instance::access(std::string_view name) const
{
    if (const auto* field = type->getField(name))
    {
        auto* ptr = field->rawAccess(value.get());
        auto aliased = std::shared_ptr<void>(value, ptr);
        return Instance { aliased, field->getType() };
    }
    throw std::runtime_error("No matching field name found");
}