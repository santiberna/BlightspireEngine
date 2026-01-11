module_declare(STATIC)

target_link_libraries(physics
        PUBLIC core
        PUBLIC application
        PUBLIC ecs
        PUBLIC resources
        
        PRIVATE renderer
        PRIVATE ImGui

        PUBLIC Jolt
)
