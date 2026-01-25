#include <impls.hpp>

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
        this->type->getDestructor()->invoke(this->ptr);
    }
}

bool Instance::isValid() const
{
    // Assert that Instance has a valid state
    assert((this->ptr == nullptr) == (this->type == nullptr));
    return ptr != nullptr;
}

bool Instance::isType(const Type& type) const
{
    return this->type == &type;
}

NO_DISCARD const Destructor* Type::getDestructor() const
{
    return destructor.get();
}

NO_DISCARD std::type_index Type::getIndex() const
{
    return { *this->index };
}