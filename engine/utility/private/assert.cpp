#include "common.hpp"

#if BB_DEVELOPMENT

#undef NDEBUG
#include <cassert>

void bb::assertImpl(bool condition)
{
    assert(condition)
}

#endif