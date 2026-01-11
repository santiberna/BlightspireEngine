module_declare(STATIC)

target_link_libraries(renderer
        PUBLIC core
        PUBLIC application
        PUBLIC particles
        PUBLIC ecs
        PUBLIC user_interface
        PUBLIC settings
        PUBLIC physics

        PUBLIC VulkanAPI
        PUBLIC spirv-reflect-static
)

source_exclude_unity("private/lib/vma_impl.cpp")