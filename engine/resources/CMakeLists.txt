module_declare(STATIC)

target_link_libraries(resources
    PUBLIC 
        core
        renderer
        thread

    PRIVATE
        fastgltf::fastgltf
)

target_include_directories(resources
        PRIVATE "${PROJECT_SOURCE_DIR}/external/stb"
)

source_exclude_unity("private/lib/stbi_impl.cpp")