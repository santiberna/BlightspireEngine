#include <factory/reflect_factory.hpp>
#include <type/builder.hpp>

namespace reflect
{

namespace detail
{
    extern ReflectFactory global_factory;
}

template <typename T> TypeBuilder<T> makeBuilder()
{
    return TypeBuilder<T>(detail::global_factory);
}

template <typename T> const Type* getType() { return detail::global_factory.get<T>(); }

template <typename T, typename... Args> Value makeValue(Args&&... args)
{
    return detail::global_factory.makeValue<T>(std::forward<Args>(args)...);
}

template <typename T> ValueRef toValueRef(T&& value)
{
    return detail::global_factory.makeRef(std::forward<T>(value));
}

template <typename... Args> ArgumentList makeArgumentList(Args&&... args)
{
    return detail::global_factory.makeArgs(std::forward<Args>(args)...);
}

}

// #define NEW_TYPE(T) TypeBuilder<T>(reflect::detail::global_factory)
// #define ADD_FIELD(field) .addField(field)