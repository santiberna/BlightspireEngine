module_declare(STATIC)

target_link_libraries(inspector
    PUBLIC
        renderer
        game
        settings
        
    PRIVATE
        pathfinding
        audio
        physics
        particles
        ImGui
)

target_include_directories(inspector
        PUBLIC "${PROJECT_SOURCE_DIR}/external/visit_struct/include"
)
