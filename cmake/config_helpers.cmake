# SETS EXECUTABLE TO OUTPUT TO A SPECIFIC DIRECTORY
function(target_output_dir executable directory)
    set_target_properties(${executable} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${directory}
    )
endfunction()

# ENABLES UNITY BUILDS ON TARGETS
function(target_enable_unity target)
    set_target_properties(${target} PROPERTIES UNITY_BUILD ON)
    set_target_properties(${target} PROPERTIES UNITY_BUILD_BATCH_SIZE 4)
endfunction()

# DISABLES UNITY BUILD ON A SPECIFIC FILE
function(source_exclude_unity file)
    set_source_files_properties(${file} PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)
endfunction()

# DISABLES PRECOMPILED HEADER ON SOURCE FILES
function(source_disable_pch files)
    set_source_files_properties(${file} PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
endfunction()

# MACRO THAT OVERRIDES CACHED OPTIONS IN CMAKE
macro(override_option option_name option_value)
    set(${option_name} ${option_value} CACHE BOOL "" FORCE)
endmacro()