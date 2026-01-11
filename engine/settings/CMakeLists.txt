module_declare(STATIC)

target_link_libraries(settings
        PUBLIC utility

        PRIVATE cereal
        PUBLIC glm::glm
)

target_include_directories(settings
        PUBLIC "${PROJECT_SOURCE_DIR}/external/visit_struct/include"
)
