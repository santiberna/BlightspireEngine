module_declare(STATIC)

target_link_libraries(bindings
        PUBLIC scripting
        PRIVATE application
        PRIVATE utility
        PRIVATE ecs
        PRIVATE audio
        PRIVATE renderer
        PRIVATE particles
        PRIVATE game
        PRIVATE pathfinding

        PRIVATE magic_enum::magic_enum
)