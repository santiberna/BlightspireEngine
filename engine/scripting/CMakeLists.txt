module_declare(STATIC)

target_link_libraries(scripting
        PUBLIC core
        PRIVATE bindings

        PUBLIC WrenBind17
        PUBLIC Wren
)