cmake_minimum_required(VERSION 3.21)

project(TKDPack VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Collect source files
file(GLOB_RECURSE TKDPACK_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
)
file(GLOB_RECURSE TKDPACK_HEADERS 
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h"
)

# Create the TKDPack executable
add_executable(TKDPack ${TKDPACK_SOURCES})

# Set target properties
set_target_properties(TKDPack PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Tools
    OUTPUT_NAME "TKDPack"
)

# Create the output directory if it doesn't exist
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Tools)

# Include directories
target_include_directories(TKDPack PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Engine/Source  # Access to TKDEngine headers
)

# Link with TKDEngineClient library
target_link_libraries(TKDPack PRIVATE
    TKDEngineClient
)

# Compiler-specific options
target_compile_options(TKDPack PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
)

# Add custom command to ensure output directory exists
add_custom_command(TARGET TKDPack POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/Tools
    COMMENT "Ensuring TKDPack output directory exists"
)
