module_declare(STATIC)

target_link_libraries(application
    PUBLIC 
        core
        steam
    PRIVATE 
        ImGui #TODO: Only necessary to forward input to imgui
        SDL3::SDL3-static
        Steamworks
)

target_include_directories(application
        PUBLIC "${PROJECT_SOURCE_DIR}/external/stb"
)
