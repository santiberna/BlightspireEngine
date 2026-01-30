#include <instance/instance.hpp>
#include <utility/argument_list.hpp>

NO_DISCARD std::vector<const Type*> ArgumentList::asTypes() const
{
    std::vector<const Type*> out {};
    out.reserve(values.size());
    for (const auto& i : values)
    {
        out.emplace_back(i->getType());
    }
    return out;
}

NO_DISCARD Instance* ArgumentList::get(size_t i) const { return this->values[i]; }