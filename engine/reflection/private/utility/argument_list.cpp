#include <utility/argument_list.hpp>
#include <value/value.hpp>

using namespace reflection;

[[nodiscard]] std::vector<const Type*> ArgumentList::asTypes() const
{
    std::vector<const Type*> out {};
    out.reserve(values.size());
    for (const auto& i : values)
    {
        out.emplace_back(i.getType());
    }
    return out;
}

[[nodiscard]] ValueRef ArgumentList::get(size_t i) const { return this->values[i]; }