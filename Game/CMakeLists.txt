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

# Create client game library
add_library(RTypeGameClient SHARED ${GAME_SOURCES})

# Set target properties for client library
set_target_properties(RTypeGameClient PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${GAME_HEADERS}"
    OUTPUT_NAME "RTypeGameClient"
    # Set the output directory for the library
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Include directories for client
target_include_directories(RTypeGameClient
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
        ${CMAKE_SOURCE_DIR}/Engine/Source  # Access to Engine headers
)

# Link libraries for client
target_link_libraries(RTypeGameClient
    PUBLIC
        TKDEngineClient  # Link against client engine (with graphics)
        sfml-system
        sfml-window
        sfml-graphics
        sfml-audio
        sfml-network
)

# Create server game library
add_library(RTypeGameServer SHARED ${GAME_SOURCES})

# Set target properties for server library
set_target_properties(RTypeGameServer PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${GAME_HEADERS}"
    OUTPUT_NAME "RTypeGameServer"
    # Set the output directory for the library
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Include directories for server
target_include_directories(RTypeGameServer
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
        ${CMAKE_SOURCE_DIR}/Engine/Source  # Access to Engine headers
)

# Link libraries for server
target_link_libraries(RTypeGameServer
    PUBLIC
        TKDEngineServer  # Link against server engine (no graphics)
)

# Compiler-specific options for client
target_compile_options(RTypeGameClient PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
)

# Compiler-specific options for server
target_compile_options(RTypeGameServer PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
)

# Create RTypeGameClient executable (Client + Game + Main.cpp from Engine)
add_executable(RTypeGameClientExe ${CMAKE_SOURCE_DIR}/Engine/Source/Main.cpp)

set_target_properties(RTypeGameClientExe PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    OUTPUT_NAME "RTypeGameClient"
)

target_include_directories(RTypeGameClientExe PRIVATE
    ${CMAKE_SOURCE_DIR}/Engine/Source
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_link_libraries(RTypeGameClientExe PRIVATE
    TKDEngineClient
    -Wl,--no-as-needed
    RTypeGameClient
    -Wl,--as-needed
)

# Create RTypeGameServer executable (Server + Game + Main.cpp from Engine)
add_executable(RTypeGameServerExe ${CMAKE_SOURCE_DIR}/Engine/Source/Main.cpp)

set_target_properties(RTypeGameServerExe PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    OUTPUT_NAME "RTypeGameServer"
)

target_include_directories(RTypeGameServerExe PRIVATE
    ${CMAKE_SOURCE_DIR}/Engine/Source
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_link_libraries(RTypeGameServerExe PRIVATE
    TKDEngineServer
    RTypeGameServer
)

# Copy resources if they exist
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Assets)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Assets 
         DESTINATION ${CMAKE_BINARY_DIR}/bin)
endif()

# Installation rules
include(GNUInstallDirs)

install(TARGETS RTypeGameClient RTypeGameServer RTypeGameClientExe RTypeGameServerExe
    EXPORT RTypeGameTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/RTypeGame
)
