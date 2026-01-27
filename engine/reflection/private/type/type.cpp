#include <type/type.hpp>

#include <cassert>
#include <constructor/constructor.hpp>
#include <instance/instance.hpp>
#include <member/field.hpp>
#include <member/method.hpp>
#include <utility/argument_list.hpp>
#include <utility/parameter_list.hpp>

Type::Type() = default;
Type::~Type() = default;

const Destructor* Type::getDestructor() const { return destructor.get(); }

std::type_index Type::getIndex() const { return { *this->index }; }

std::optional<uint64_t> Type::getConstant(std::string_view name) const
{
    if (auto it = this->constants.find(std::string(name)); it != this->constants.end())
    {
        return it->second;
    }
    return std::nullopt;
}

Instance Type::construct(const ArgumentList& args) const
{
    auto params = args.asTypes();
    auto list = ParameterList(params);

    if (auto it = constructors.find(list); it != constructors.end())
    {
        assert(owner_store != nullptr);
        return it->second->invoke(*owner_store, args);
    }
    return {};
}