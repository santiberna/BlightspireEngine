module_declare(STATIC)

target_link_libraries(application
        PUBLIC core
        PUBLIC steam

        PUBLIC VulkanAPI #TODO: Might be able to remove this dependency
        PRIVATE ImGui #TODO: Only necessary to forward input to imgui
        PRIVATE SDL3::SDL3-static
        PRIVATE Steamworks
)

target_include_directories(application
        PUBLIC "${PROJECT_SOURCE_DIR}/external/stb"
)
