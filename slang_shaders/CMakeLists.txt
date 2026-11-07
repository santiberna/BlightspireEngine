find_program(SLANGC slangc HINTS /usr/bin /usr/local/bin $ENV{VULKAN_SDK}/Bin/ $ENV{VULKAN_SDK}/Bin32/)
find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(BB_SLANG_DEBUG_SYMBOLS 0 CACHE STRING "The debug level the shaders are generated with [0-3]")
set(BB_SLANG_OPTIMIZATION_LEVEL 0 CACHE STRING "The optimization level the shaders are generated with [0-3]") # Optimization level >0 removes unused variables which breaks our descriptor layout system.

if (NOT SLANGC)
    message(FATAL_ERROR "slangc not found at ${SLANGC}")
endif ()

set(SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE SLANG_SHADERS CONFIGURE_DEPENDS
        ${SHADER_DIR}/*.slang
)

# BIN_DIR is the output for the .spv files
set(BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin")
file(MAKE_DIRECTORY ${BIN_DIR}) 

# SHADER_MODULE_DIR is the output for the slang-module files. These will be linked together into spv binary.
set(MODULE_DIR "${CMAKE_CURRENT_BINARY_DIR}/slang-modules")
file(MAKE_DIRECTORY ${MODULE_DIR})

function(set_shader_paths SHADER)
    file(RELATIVE_PATH SHADER_REL ${SHADER_DIR} ${SHADER})
    get_filename_component(SHADER_NAME ${SHADER_REL} NAME_WLE)
    get_filename_component(SHADER_REL_DIR ${SHADER_REL} DIRECTORY)
    set(SHADER_REL ${SHADER_REL_DIR}/${SHADER_NAME})
    set(SHADER_SPV ${BIN_DIR}/${SHADER_REL}.spv)
    set(SHADER_MODULE ${MODULE_DIR}/${SHADER_REL}.slang-module)
    set(SHADER_MODULE_DIR ${MODULE_DIR}/${SHADER_REL_DIR})
    set(SHADER_DEPFILE ${SHADER_MODULE}.deps)

    set(SHADER_REL ${SHADER_REL} PARENT_SCOPE)
    set(SHADER_SPV ${SHADER_SPV} PARENT_SCOPE)
    set(SHADER_MODULE ${SHADER_MODULE} PARENT_SCOPE)
    set(SHADER_MODULE_DIR ${SHADER_MODULE_DIR} PARENT_SCOPE)
    set(SHADER_DEPFILE ${SHADER_DEPFILE} PARENT_SCOPE)
endfunction()

# Process all individual slang files into slang-modules
foreach (SLANG_SHADER ${SLANG_SHADERS})
    set_shader_paths(${SLANG_SHADER})

    # Make sure a destination folder exists.
    file(MAKE_DIRECTORY ${SHADER_MODULE_DIR})

    add_custom_command(
        OUTPUT ${SHADER_MODULE}
        COMMAND "${SLANGC}" ${SLANG_SHADER} -o ${SHADER_MODULE} -depfile ${SHADER_DEPFILE} -I ${SHADER_DIR}
        COMMAND_EXPAND_LISTS
        COMMENT "Compiling ${SLANG_SHADER} to slang-module"
        DEPFILE ${SHADER_DEPFILE}
        BYPRODUCTS ${SHADER_DEPFILE}
        VERBATIM
    )

    list(APPEND SLANG_MODULES ${SHADER_MODULE})
endforeach ()

list(APPEND ENTRY_POINTS main_vert)
list(APPEND ENTRY_POINTS main_frag)
list(APPEND ENTRY_POINTS main_comp)

# Find what entry points this shader has (e.g. main_vert, main_frag, etc.)
# and store them in the list ENTRY_POINTS with -entry prefixed
function(gather_entry_points shader)
    file(READ ${shader} SHADER_SOURCE)

    foreach(ENTRY_POINT ${ENTRY_POINTS})
        string(FIND "${SHADER_SOURCE}" ${ENTRY_POINT} INDEX)
        if(NOT INDEX EQUAL -1)
            list(APPEND FOUND_ENTRY_POINTS -entry)
            list(APPEND FOUND_ENTRY_POINTS ${ENTRY_POINT})
        endif()
    endforeach()

    list(LENGTH FOUND_ENTRY_POINTS FOUND_ENTRY_POINTS_LENGTH)

    set(FOUND_ENTRY_POINTS ${FOUND_ENTRY_POINTS} PARENT_SCOPE)
    set(FOUND_ENTRY_POINTS_LENGTH ${FOUND_ENTRY_POINTS_LENGTH} PARENT_SCOPE)
endfunction()

SET(PY_COMPILE_SHADER ${CMAKE_SOURCE_DIR}/scripts/compile_shader.py)

foreach (SHADER ${SLANG_SHADERS})
    set_shader_paths(${SHADER})

    gather_entry_points(${SHADER})
    if(FOUND_ENTRY_POINTS_LENGTH EQUAL 0)
        continue()
    endif()


    add_custom_command(
        OUTPUT "${SHADER_SPV}"
        COMMAND
            ${Python3_EXECUTABLE}
            "${PY_COMPILE_SHADER}"
            "--slangc=${SLANGC}"
            "--dep-file=${SHADER_DEPFILE}"
            "--shader-output=${SHADER_SPV}"
            "--shader-module=${SHADER_MODULE}"
            "--shader=${SHADER}"
            "--shader-dir=${SHADER_DIR}"
            "--shader-module-dir=${MODULE_DIR}"
            "--debug-level=${BB_SLANG_DEBUG_SYMBOLS}"
            "--optimization-level=${BB_SLANG_OPTIMIZATION_LEVEL}"
        DEPENDS
            ${SHADER}
            ${SHADER_MODULE}
            ${SHADER_DEPFILE}
            ${PY_COMPILE_SHADER} 
        VERBATIM
    )

    
    list(APPEND SPV_FILES ${SHADER_SPV})
    set(SPV_FILES ${SPV_FILES} PARENT_SCOPE)
endforeach()

add_custom_target(SlangShaders ALL DEPENDS ${SLANG_MODULES} ${SPV_FILES})

# ${PYTHON_SCRIPT})

#set(REFLECTED_HEADER_OUTPUT "${PROJECT_SOURCE_DIR}/engine/renderer/public/reflected_glsl.hpp")
#set(PYTHON_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/scripts/reflect.py")
#
#find_package(Python3 REQUIRED)
#
#message("${Python3_EXECUTABLE} \"${PYTHON_SCRIPT}\" \"${REFLECTED_HEADER_OUTPUT}\"")
#
#add_custom_command(
#        OUTPUT ${REFLECTED_HEADER_OUTPUT}
#        COMMAND ${Python3_EXECUTABLE} ${PYTHON_SCRIPT} ${REFLECTED_HEADER_OUTPUT}
#        DEPENDS ${PYTHON_SCRIPT} ${SHADERS}
#        COMMENT "Reflecting glsl shaders to c++ header"
#        VERBATIM
#)
