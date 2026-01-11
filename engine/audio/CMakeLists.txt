module_declare(STATIC)

target_link_libraries(audio
        PUBLIC core
        PUBLIC ecs
        PRIVATE physics
        
        PRIVATE FMOD
)
