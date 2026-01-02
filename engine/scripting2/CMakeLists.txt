add_library(Scripting2)
module_default_init(Scripting2)

target_link_libraries(Scripting2
        PRIVATE Utility
        PRIVATE angelscript
        PRIVATE angelscript-addons
)