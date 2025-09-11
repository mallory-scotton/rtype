cmake_minimum_required(VERSION 3.21)

project(RTypeServer VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Find dependencies
find_package(SFML 2.6.1 REQUIRED COMPONENTS system network)

# Collect source files
file(GLOB_RECURSE SERVER_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
)

file(GLOB_RECURSE SERVER_HEADERS 
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h"
)

# Create executable
add_executable(RTypeServer ${SERVER_SOURCES})

# Include directories
target_include_directories(RTypeServer
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
        ${CMAKE_SOURCE_DIR}/Engine/Source  # Access to Engine headers
)

# Link libraries
target_link_libraries(RTypeServer
    PRIVATE
        RTypeEngine  # Link against our engine
        sfml-system
        sfml-network
)

# Compiler-specific options
target_compile_options(RTypeServer PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
)

# Set output directory
set_target_properties(RTypeServer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Installation rules
include(GNUInstallDirs)

install(TARGETS RTypeServer
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
