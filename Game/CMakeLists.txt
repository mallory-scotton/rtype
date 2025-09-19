cmake_minimum_required(VERSION 3.21)

project(RTypeGame VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Enable position independent code for shared library
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Find dependencies
find_package(SFML 2.6.1 REQUIRED COMPONENTS system window graphics audio network)

# Collect source files
file(GLOB_RECURSE GAME_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
)

file(GLOB_RECURSE GAME_HEADERS 
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h"
)

# Create shared library
add_library(RTypeGame SHARED ${GAME_SOURCES})

# Set target properties for library
set_target_properties(RTypeGame PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${GAME_HEADERS}"
    OUTPUT_NAME "RTypeGame"
    # Set the output directory for the library
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Include directories
target_include_directories(RTypeGame
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
        ${CMAKE_SOURCE_DIR}/Engine/Source  # Access to Engine headers
)

# Link libraries
target_link_libraries(RTypeGame
    PUBLIC
        TKDEngineClient  # Link against client engine (with graphics)
        sfml-system
        sfml-window
        sfml-graphics
        sfml-audio
        sfml-network
)

# Compiler-specific options
target_compile_options(RTypeGame PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
)

# Create RTypeGameClient executable (Client + Game + Main.cpp from Engine)
add_executable(RTypeGameClient ${CMAKE_SOURCE_DIR}/Engine/Source/Main.cpp)

set_target_properties(RTypeGameClient PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

target_include_directories(RTypeGameClient PRIVATE
    ${CMAKE_SOURCE_DIR}/Engine/Source
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_link_libraries(RTypeGameClient PRIVATE
    TKDEngineClient
    RTypeGame
)

# Create RTypeGameServer executable (Server + Game + Main.cpp from Engine)
add_executable(RTypeGameServer ${CMAKE_SOURCE_DIR}/Engine/Source/Main.cpp)

set_target_properties(RTypeGameServer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

target_include_directories(RTypeGameServer PRIVATE
    ${CMAKE_SOURCE_DIR}/Engine/Source
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_link_libraries(RTypeGameServer PRIVATE
    TKDEngineServer
    RTypeGame
)

# Copy resources if they exist
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Assets)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Assets 
         DESTINATION ${CMAKE_BINARY_DIR}/bin)
endif()

# Installation rules
include(GNUInstallDirs)

install(TARGETS RTypeGame RTypeGameClient RTypeGameServer
    EXPORT RTypeGameTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/RTypeGame
)
