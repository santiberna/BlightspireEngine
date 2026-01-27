#include <impls.hpp>

#include <member/constant.hpp>

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

bool Instance::isType(const Type& type) const { return this->type == &type; }

const Destructor* Type::getDestructor() const { return destructor.get(); }

std::type_index Type::getIndex() const { return { *this->index }; }

const void* Instance::getAddress() const { return this->ptr; }

void Type::setConstant(std::string_view name, uint64_t value)
{
    auto name_str = std::string(name);
    constants.emplace(name_str, Constant(name_str, value));
}

NO_DISCARD const Constant* Type::getConstant(std::string_view name) const
{
    if (auto it = constants.find(std::string(name)); it != constants.end())
    {
        return &it->second;
    }
    return nullptr;
}