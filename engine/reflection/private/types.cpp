#include <impls.hpp>

Type::Type(TypeStore* owner_store)
    : owner_store(owner_store)
{
}

Instance::Instance(Instance&& other) noexcept
{
    std::swap(other.ptr, this->ptr);
    std::swap(other.type, this->type);
}

Instance& Instance::operator=(Instance&& other) noexcept
{
    std::swap(other.ptr, this->ptr);
    std::swap(other.type, this->type);
    return *this;
}

Instance::~Instance()
{
    // Assert that Instance has a valid state
    assert((this->ptr == nullptr) == (this->type == nullptr));
    if (this->ptr != nullptr && this->type != nullptr)
    {
        this->type->getDestructor()->invokeDelete(this->ptr);
    }
}

bool Instance::isValid() const
{
    // Assert that Instance has a valid state
    assert((this->ptr == nullptr) == (this->type == nullptr));
    return ptr != nullptr;
}

const Type* Instance::getType() const { return this->type; }

const Destructor* Type::getDestructor() const { return destructor.get(); }

std::type_index Type::getIndex() const { return { *this->index }; }

const void* Instance::getAddress() const { return this->ptr; }

std::optional<uint64_t> Type::getConstant(std::string_view name) const
{
    if (auto it = this->constants.find(std::string(name)); it != this->constants.end())
    {
        return it->second;
    }
    return std::nullopt;
}

Instance Type::construct(const ArgumentList& arguments) const
{
    auto params = arguments.asTypes();
    auto list = ParameterList(params);

    if (auto it = constructors.find(list); it != constructors.end())
    {
        assert(owner_store != nullptr);
        return it->second->invoke(*owner_store, arguments);
    }
    return {};
}