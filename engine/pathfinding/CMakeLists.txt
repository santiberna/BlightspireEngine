module_declare(STATIC)

target_link_libraries(pathfinding
        PUBLIC core
        PUBLIC application
        PUBLIC physics
        PUBLIC renderer
        PUBLIC resources
        PRIVATE game
)
