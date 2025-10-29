cmake_minimum_required(VERSION 3.21)

project(TKDEmbed VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Collect source files
file(GLOB_RECURSE TKDEMBED_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
)
file(GLOB_RECURSE TKDEMBED_HEADERS 
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h"
)

# Create the TKDEmbed executable
add_executable(TKDEmbed ${TKDEMBED_SOURCES})

# Set target properties
set_target_properties(TKDEmbed PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Tools
    OUTPUT_NAME "TKDEmbed"
)

# For multi-config generators, remove the config subdirectory
foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${CONFIG} CONFIG_UPPER)
    set_target_properties(TKDEmbed PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/bin/Tools
    )
endforeach()

# Create the output directory if it doesn't exist
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Tools)

# Include directories
target_include_directories(TKDEmbed PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Engine/Source  # Access to TKDEngine headers
)

# Link with TKDEngineClient library
target_link_libraries(TKDEmbed PRIVATE
    TKDEngineClient
)

# Compiler-specific options
target_compile_options(TKDEmbed PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
)

# Add custom command to ensure output directory exists
add_custom_command(TARGET TKDEmbed POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/Tools
    COMMENT "Ensuring TKDEmbed output directory exists"
)

# Copy required DLLs to the tool directory on Windows
if(WIN32)
    add_custom_command(TARGET TKDEmbed POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:TKDEngineClient>
            $<TARGET_FILE_DIR:TKDEmbed>
        COMMENT "Copying TKDEngineClient.dll to TKDEmbed directory"
    )
endif()
