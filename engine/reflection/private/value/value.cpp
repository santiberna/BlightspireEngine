#include <value/value.hpp>

#include <member/method.hpp>
#include <value/value_ref.hpp>

using namespace reflect;
using namespace reflect::detail;

Value::Value(MAYBE_UNUSED VoidValueType _unused) { }

bool Value::operator==(MAYBE_UNUSED VoidValueType _unused) const { return value == nullptr; }

Value::Value(std::shared_ptr<void> value, const Type* type)
    : value(std::move(value))
    , type(type)
{
}

ValueRef Value::asRef() const { return { this->value.get(), this->type }; }

Value Value::call(std::string_view name, const ArgumentList& args) const
{
    return this->asRef().call(name, args);
}

const Type* Value::getType() const { return this->type; }

ValueRef Value::access(std::string_view name) const { return this->asRef().access(name); }