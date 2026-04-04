module_declare(STATIC)

target_link_libraries(application
    PUBLIC 
        core
        steam
        
    PRIVATE 
        SDL3::SDL3-static
        Steamworks

)

if (BB_DEVELOPMENT)
    target_link_libraries(application PRIVATE inspector)
endif()

target_include_directories(application
        PUBLIC "${PROJECT_SOURCE_DIR}/external/stb"
)
