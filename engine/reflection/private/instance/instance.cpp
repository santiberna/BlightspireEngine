#include <instance/instance.hpp>

#include <destructor/destructor.hpp>
#include <type/type.hpp>

#include <cassert>
#include <utility>

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

const void* Instance::getAddress() const { return this->ptr; }
