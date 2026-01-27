#include <utility/argument_list.hpp>
#include <utility/parameter_list.hpp>

#include <instance.hpp>

NO_DISCARD bool ParameterList::validateArgs(const ArgumentList& args) const
{
    auto as_types = args.asTypes();
    return this->types == as_types;
}

std::size_t std::hash<ParameterList>::operator()(const ParameterList& params) const noexcept
{
    std::size_t hash = 0;
    for (const auto* type : params.types)
    {
        // Combine hashes
        hash ^= std::hash<const Type*> {}(type) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
    return hash;
}
