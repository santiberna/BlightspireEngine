module_declare(STATIC)

target_link_libraries(inspector
        PUBLIC renderer
        PRIVATE particles
        PUBLIC game
        PUBLIC settings
        PRIVATE physics
        PRIVATE pathfinding
        PRIVATE audio

        PRIVATE ImGui
)

target_include_directories(inspector
        PUBLIC "${PROJECT_SOURCE_DIR}/external/visit_struct/include"
)
