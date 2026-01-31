#include <instance/instance.hpp>

#include <instance/instance_ref.hpp>
#include <member/method.hpp>

Instance::Instance(MAYBE_UNUSED VoidInstanceType _unused) { }

bool Instance::operator==(MAYBE_UNUSED VoidInstanceType _unused) const { return value == nullptr; }

Instance::Instance(std::shared_ptr<void> value, const Type* type)
    : value(std::move(value))
    , type(type)
{
}

InstanceRef Instance::asRef() const { return { this->value.get(), this->type }; }

Instance Instance::call(std::string_view name, const ArgumentList& args) const
{
    return this->asRef().call(name, args);
}

const Type* Instance::getType() const { return this->type; }

InstanceRef Instance::access(std::string_view name) const { return this->asRef().access(name); }