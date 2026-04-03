add_executable(Importer)
target_sources(Importer PRIVATE "main.cpp")
target_output_dir(Importer ${CMAKE_BINARY_DIR}/tools)

target_link_libraries(Importer
    PRIVATE
        CompilerFlags
        stb
        fastgltf::fastgltf
        cereal::cereal
        spdlog::spdlog
)