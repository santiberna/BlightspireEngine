#include <member/field.hpp>

using namespace reflect;

Field::~Field() { };
void* Field::rawAccess(void* ptr) const { return (std::byte*)ptr + offset; };
const Type* Field::getType() const { return type; }