#include <utility/argument_list.hpp>
#include <utility/parameter_list.hpp>

using namespace reflection;

NO_DISCARD bool ParameterList::validateArgs(const ArgumentList& args) const
{
    auto as_types = args.asTypes();
    return this->types == as_types;
}

std::size_t std::hash<ParameterList>::operator()(const ParameterList& params) const noexcept
{
    constexpr size_t FLAT_ADD = 0x9e3779b9;
    constexpr size_t RSHIFT = 2;
    constexpr size_t LFSHIFT = 6;
    std::size_t hash = 0;
    for (const auto* type : params.types)
    {
        // Combine hashes
        hash ^= std::hash<const Type*> {}(type) + FLAT_ADD + (hash << LFSHIFT) + (hash >> RSHIFT);
    }
    return hash;
}
