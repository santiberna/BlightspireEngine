module_declare(STATIC)

target_link_libraries(ecs
        PUBLIC core
        PUBLIC scripting
        
        PRIVATE physics

        PUBLIC EnTT::EnTT
        PUBLIC cereal
        PUBLIC ImGui
)
