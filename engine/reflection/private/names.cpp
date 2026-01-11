#include <array>
#include <common.hpp>
#include <names.hpp>

std::string_view sanitizeTypeName(std::string_view mangled_name)
{
#if defined(WINDOWS)
    std::array<std::string_view, 2> prefixes = {
        "struct ",
        "class ",
    };

    for (const auto& prefix : prefixes)
    {
        if (mangled_name.starts_with(prefix))
        {
            mangled_name = mangled_name.substr(prefix.length());
            break;
        }
    }

    return mangled_name;
#elif defined(LINUX)
#error "Unimplemented function for this platform"
#endif
}