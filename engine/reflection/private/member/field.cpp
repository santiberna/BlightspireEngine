#include <member/field.hpp>

Field::~Field() = default;
void* Field::rawAccess(void* ptr) const { return (std::byte*)ptr + offset; };
const Type* Field::getType() const { return type; }