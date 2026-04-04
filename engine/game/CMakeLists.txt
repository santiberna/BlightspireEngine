module_declare(STATIC)

target_link_libraries(game
        PUBLIC core
        PUBLIC ecs
        PUBLIC user_interface
        PUBLIC renderer
        PUBLIC resources
        PUBLIC steam
        PRIVATE audio
        PRIVATE pathfinding
        PUBLIC ImGui
)
