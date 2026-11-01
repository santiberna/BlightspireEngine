find_program(SLANGC slangc HINTS /usr/bin /usr/local/bin $ENV{VULKAN_SDK}/Bin/ $ENV{VULKAN_SDK}/Bin32/)

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

# INTERMEDIATE_DIR is the output for the slang-module files. These will be linked to gether into spv binary.
set(INTERMEDIATE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/intermediate")
file(MAKE_DIRECTORY ${INTERMEDIATE_DIR})

# Process all individual slang files into slang-modules
foreach (SLANG_SHADER ${SLANG_SHADERS})
    # Get the relative path of the file based on the shader directory
    file(RELATIVE_PATH SLANG_SHADER_REL ${SHADER_DIR} ${SLANG_SHADER})
    get_filename_component(SLANG_SHADER_REL_PATH ${SLANG_SHADER_REL} DIRECTORY)

    # Set where the output of the .slang-module goes. Here we mimic the original folder structure.
    set(SLANG_OUTPUT "${INTERMEDIATE_DIR}/${SLANG_SHADER_REL}-module")
    get_filename_component(SLANG_SHADER_OUTPUT_DIR ${SLANG_OUTPUT} DIRECTORY)

    # Make sure a desitination folder exists.
    file(MAKE_DIRECTORY ${SLANG_OUTPUT_DIR})

    # Will output the dependencies of the file. Useful for determining module dependencies.
    set(DEP_FILE ${SLANG_OUTPUT}.deps)

    add_custom_command(
        OUTPUT ${SLANG_OUTPUT} ${DEP_FILE}
        COMMAND "${SLANGC}" ${SLANG_SHADER} -o ${SLANG_OUTPUT} -depfile ${DEP_FILE}
        COMMAND_EXPAND_LISTS
        COMMENT "Compiling ${SLANG_SHADER} to slang-module"
        DEPENDS ${SLANG_SHADER}
        VERBATIM
    )

    list(APPEND SLANG_MODULES ${SLANG_OUTPUT})
endforeach ()

list(APPEND ENTRY_POINTS main_vert)
list(APPEND ENTRY_POINTS main_frag)
list(APPEND ENTRY_POINTS main_comp)

# Find what entry points this shader has (e.g. main_ver, main_frag, etc.)
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

foreach (SHADER ${SLANG_SHADERS})
    get_filename_component(FILE_NAME ${SHADER} NAME_WE)
    file(RELATIVE_PATH SHADER_REL ${SHADER_DIR} ${SHADER})
    set(SHADER_OUTPUT "${BIN_DIR}/${FILE_NAME}.spv")
    set(SHADER_MODULE ${INTERMEDIATE_DIR}/${SHADER_REL}-module)
    set(SHADER_DEPFILE ${SHADER_MODULE}.deps)

    gather_entry_points(${SHADER})
    if(FOUND_ENTRY_POINTS_LENGTH EQUAL 0)
        continue()
    endif()

    add_custom_command(
        OUTPUT "${SHADER_OUTPUT}"
        COMMAND ${CMAKE_COMMAND}
            "-DDEP_FILE=${SHADER_DEPFILE}"
            "-DSHADER_OUTPUT=${SHADER_OUTPUT}"
            "-DSHADER_MODULE=${SHADER_MODULE}"
            "-DSHADER=${SHADER}"
            "-DSHADER_DIR=${SHADER_DIR}"
            "-DINTERMEDIATE_DIR=${INTERMEDIATE_DIR}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/compile_shader.cmake"
        DEPENDS
            ${SHADER}
            ${SHADER_MODULE}
            ${SHADER_DEPFILE}
        DEPFILE ${SHADER_DEPFILE}
        VERBATIM
    )
    
    list(APPEND SPV_FILES ${SHADER_OUTPUT})
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
