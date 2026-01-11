module_declare(STATIC)

target_link_libraries(utility
        PUBLIC magic_enum::magic_enum
        PUBLIC Tracy::TracyClient
        PUBLIC spdlog::spdlog
        PUBLIC glm::glm
        PUBLIC cereal::cereal
        PUBLIC physfs-static
)

target_include_directories(utility
        PUBLIC "${PROJECT_SOURCE_DIR}/external/visit_struct/include"
        PUBLIC "${PHYSFS_SOURCE_DIR}/src"
        PUBLIC "${PROJECT_SOURCE_DIR}/external/stb"
)
