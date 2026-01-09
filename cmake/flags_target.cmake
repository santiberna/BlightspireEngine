# Contains relevant compiler flags for all source files in the codebase
add_library(CompilerFlags INTERFACE)

target_compile_options(CompilerFlags
        INTERFACE -Wall INTERFACE -Wextra INTERFACE -Wno-unknown-pragmas)

if (WARNINGS_AS_ERRORS)
    target_compile_options(CompilerFlags INTERFACE -Werror)
endif ()

# Define if it's the distribution config
if ("${CMAKE_PRESET_NAME}" STREQUAL "x64-Distribution" OR "${CMAKE_PRESET_NAME}" STREQUAL "WSL-Distribution")
    target_compile_definitions(CompilerFlags INTERFACE DISTRIBUTION)
endif ()