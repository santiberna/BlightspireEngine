# Contains relevant compiler flags for all source files in the codebase
add_library(CompilerFlags INTERFACE)

target_compile_options(CompilerFlags
    INTERFACE
        -Wall 
        -Wextra
        -Wno-unknown-pragmas)

if (WARNINGS_AS_ERRORS)
    target_compile_options(CompilerFlags INTERFACE -Werror)
endif ()

# Define if it's the distribution config
if (DEVELOPER_BUILD)
    target_compile_definitions(CompilerFlags INTERFACE BB_DEVELOPMENT)
endif ()