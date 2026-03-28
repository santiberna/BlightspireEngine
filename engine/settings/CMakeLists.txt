module_declare(OBJECT)

target_link_libraries(settings
    PUBLIC 
        utility
)

target_include_directories(settings
    PUBLIC
        "${PROJECT_SOURCE_DIR}/external/visit_struct/include"
)
