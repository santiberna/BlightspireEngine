# THIS DIRECTORY IS ONLY ADDED IF TESTS ARE ENABLED IN CMAKE CONFIG
add_executable(UnitTests main.cpp)
target_output_dir(UnitTests ${CMAKE_BINARY_DIR})

# Google Test
FetchContent_Declare(
        GoogleTest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.16.0
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
override_option(gtest_force_shared_crt ON)
FetchContent_MakeAvailable(GoogleTest)

target_compile_options(gtest PRIVATE -Wno-character-conversion)

target_link_libraries(UnitTests
        PRIVATE CompilerFlags
        PRIVATE GTest::gtest
)

include(GoogleTest)
gtest_discover_tests(UnitTests)