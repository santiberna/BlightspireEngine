module_declare(STATIC)

# Linked libraries or other modules (use PUBLIC for propagating includes)
target_link_libraries(user_interface
        PUBLIC utility
        PUBLIC core
        PUBLIC scripting
        PRIVATE application
        PRIVATE game

        PUBLIC freetype)