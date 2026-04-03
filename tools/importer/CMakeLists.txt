add_executable(importer)
target_sources(importer PRIVATE "main.cpp")
target_output_dir(importer ${CMAKE_BINARY_DIR}/tools)

target_link_libraries(importer
    PRIVATE
        CompilerFlags
        stb
        fastgltf::fastgltf
        cereal::cereal
)