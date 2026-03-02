#include <member/field.hpp>

using namespace reflection;

Field::~Field() { };
void* Field::rawAccess(void* ptr) const { return (std::byte*)ptr + offset; };
const Type* Field::getType() const { return type; }