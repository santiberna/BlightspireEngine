module_declare(STATIC)

target_link_libraries(particles
        PUBLIC core
        PUBLIC ecs
        PUBLIC settings
        PRIVATE utility
        PRIVATE renderer
        PRIVATE physics
)