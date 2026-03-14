#pragma once

#include <reflection.hpp>

#define NEW_TYPE(TYPE) reflection::TypeBuilder<TYPE>(reflection::detail::global_factory)
#define ADD_CONSTRUCTOR(ARGS) .addConstructor<ARGS>()
#define ADD_FIELD(FIELD) .addField(NAMEOF(FIELD), FIELD)
#define ADD_METHOD(METHOD) .addMethod(NAMEOF(METHOD), METHOD)
#define ADD_CONSTANT(VALUE) .addConstant(NAMEOF(VALUE), VALUE)

/// Helper macro to concatenate tokens
#define DETAIL_CONCAT_IMPL(x, y) x##y
#define DETAIL_CONCAT(x, y) DETAIL_CONCAT_IMPL(x, y)

#define DETAIL_STATIC_BLOCK_IMPL(id)                 \
    namespace                                        \
    {                                                \
    struct DETAIL_CONCAT(_StaticInitializer, id)     \
    {                                                \
        DETAIL_CONCAT(_StaticInitializer, id)();     \
    } DETAIL_CONCAT(_static_init_instance, id) = {}; \
    }                                                \
    DETAIL_CONCAT(_StaticInitializer, id)::DETAIL_CONCAT(_StaticInitializer, id)()

/// Macro for creating a static initialization block
/// Code in the scope will run before main()
#define STATIC_BLOCK DETAIL_STATIC_BLOCK_IMPL(__COUNTER__)