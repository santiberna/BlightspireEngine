#include <member/field.hpp>

using namespace reflect;
using namespace reflect::detail;

Field::~Field() = default;
void* Field::rawAccess(void* ptr) const { return (std::byte*)ptr + offset; };
const Type* Field::getType() const { return type; }