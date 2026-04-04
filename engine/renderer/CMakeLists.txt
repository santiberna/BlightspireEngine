module_declare(STATIC)

target_link_libraries(renderer
    PUBLIC 
        core
        application
        particles
        ecs
        user_interface
        settings
        physics
        VulkanAPI
        spirv-reflect-static
        ImGui

    PRIVATE
        SDL3::SDL3-static
)

source_exclude_unity("private/lib/vma_impl.cpp")