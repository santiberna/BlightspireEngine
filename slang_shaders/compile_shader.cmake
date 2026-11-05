file(READ ${DEP_FILE} DEP_CONTENT)
string(STRIP "${DEP_CONTENT}" DEP_CONTENT)
string(REPLACE " " ";" DEPENDENCIES ${DEP_CONTENT})
list(POP_FRONT DEPENDENCIES) # Removes the header (is just the path of the shader-module)
list(POP_FRONT DEPENDENCIES) # Removes the source slang file this is dependent on

foreach(DEPENDENCY ${DEPENDENCIES})
    string(REPLACE "\\\\" "\\" DEPENDENCY ${DEPENDENCY})
    string(REPLACE "\\:" ":" DEPENDENCY ${DEPENDENCY})
    file(RELATIVE_PATH SHADER_REL ${SHADER_DIR} ${DEPENDENCY})
    list(APPEND MODULE_DEPENDENCIES ${INTERMEDIATE_DIR}/${SHADER_REL}-module)
endforeach()

# Enable debug symbols for Debug builds
if(CMAKE_BUILD_TYPE EQUAL Debug)
    set(DEBUG_CMD -g3)
else()
    set(DEBUG_CMD -g0)
endif()

if(CMAKE_BUILD_TYPE EQUAL Release)
    set(OPT_CMD -O3)
else()
    set(OPT_CMD -O0)
endif()

execute_process(
    COMMAND slangc
        ${MODULE_DEPENDENCIES}
        ${SHADER_MODULE}
        -target spirv
        -capability SPV_EXT_descriptor_indexing
        -o ${SHADER_OUTPUT}
        ${DEBUG_CMD} ${OPT_CMD}
    RESULT_VARIABLE RESULT
    COMMAND_ECHO STDOUT
)

if(NOT RESULT EQUAL 0)
    message(FATAL_ERROR "Shader compilation failed")
endif()