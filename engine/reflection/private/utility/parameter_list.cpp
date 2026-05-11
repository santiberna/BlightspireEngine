#include <utility/argument_list.hpp>
#include <utility/parameter_list.hpp>

using namespace reflection;

[[nodiscard]] bool ParameterList::validateArgs(const ArgumentList& args) const
{
    auto as_types = args.asTypes();
    return this->types == as_types;
}

bb::usize std::hash<ParameterList>::operator()(const ParameterList& params) const noexcept
{
    constexpr bb::usize FLAT_ADD = 0x9e3779b9;
    constexpr bb::usize RSHIFT = 2;
    constexpr bb::usize LFSHIFT = 6;
    bb::usize hash = 0;
    for (const auto* type : params.types)
    {
        // Combine hashes
        hash ^= std::hash<const Type*> {}(type) + FLAT_ADD + (hash << LFSHIFT) + (hash >> RSHIFT);
    }
    return hash;
}
