

# FUNCTION THAT DECLARES ALL THE DEFAULTS OF A MODULE
function(module_declare type)
    get_filename_component(module ${CMAKE_CURRENT_SOURCE_DIR} NAME)

    file(GLOB_RECURSE public_headers CONFIGURE_DEPENDS "public/*.hpp")
    file(GLOB_RECURSE private_sources CONFIGURE_DEPENDS "private/*.cpp")
    file(GLOB_RECURSE private_headers CONFIGURE_DEPENDS "private/*.hpp")

    add_library(${module} ${type})
    target_sources(${module} PUBLIC ${public_files} PRIVATE ${private_headers} PRIVATE ${private_sources})
    target_include_directories(${module} PUBLIC "public" PRIVATE "private")

    target_link_libraries(${module} PRIVATE CompilerFlags)

    if (ENABLE_PCH)
        target_link_libraries(${module} PRIVATE PCH)
        target_precompile_headers(${module} REUSE_FROM PCH)
    endif ()

    if (COMPILE_TESTS)
        file(GLOB_RECURSE test_sources CONFIGURE_DEPENDS "tests/*.cpp")

        if (test_sources)
            target_sources(${module} PRIVATE ${test_sources})
            target_link_libraries(${module} PRIVATE GTest::gtest)
            target_link_libraries(UnitTests PRIVATE $<LINK_LIBRARY:WHOLE_ARCHIVE,${module}>)
        endif ()

    endif ()

    if (ENABLE_UNITY)
        target_enable_unity(${module})
    endif ()

endfunction()

# FUNCTION THAT ADDS A MODULE TO THE MAIN ENGINE LIBRARY
function(target_add_module target directory)
    get_filename_component(module ${directory} NAME)

    add_subdirectory(${directory})
    message(STATUS "### - ${module} Module")
    target_link_libraries(${target} INTERFACE ${module})
endfunction()
