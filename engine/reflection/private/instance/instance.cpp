#include <instance/instance.hpp>

Instance::Instance(MAYBE_UNUSED VoidInstanceType _unused) { }

bool Instance::operator==(MAYBE_UNUSED VoidInstanceType _unused) const { return value == nullptr; }

Instance::Instance(std::shared_ptr<void> value, const Type* type)
    : value(std::move(value))
    , type(type)
{
}

const Type* Instance::getType() const { return this->type; }

Instance Instance::access(std::string_view name) const
{
    if (const auto* field = type->getField(name))
    {
        auto* ptr = field->apply_raw_offset(value.get());
        auto aliased = std::shared_ptr<void>(value, ptr);
        return Instance { aliased, field->getType() };
    }
    throw std::runtime_error("No matching field name found");
}