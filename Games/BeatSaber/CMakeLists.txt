cmake_minimum_required(VERSION 3.21)

project(BeatSaberGame VERSION 1.0.0 LANGUAGES CXX)

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
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenVR REQUIRED)

# Define tool paths
set(ASSETS_DIR "Assets")
set(CONTENT_PAK "${CMAKE_BINARY_DIR}/bin/Data/BeatSaberGameContent.pak")

# Custom command to generate packed content
add_custom_command(
    OUTPUT ${CONTENT_PAK}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/Data
    COMMAND $<TARGET_FILE:TKDPack> -i ${ASSETS_DIR} -o ${CONTENT_PAK} -m pack
    DEPENDS TKDPack
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating packed content from Assets directory"
    VERBATIM
)

# Create a custom target for packed content (unique per-game to avoid name collisions)
add_custom_target(ContentPak_BeatSaber
    DEPENDS ${CONTENT_PAK} TKDPack
)

# Collect source files
file(GLOB_RECURSE GAME_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
)

file(GLOB_RECURSE GAME_HEADERS 
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h"
)

# Create client game library
add_library(BeatSaberGameClient SHARED ${GAME_SOURCES})

# Add dependency on ContentPak target (use unique per-game target name)
add_dependencies(BeatSaberGameClient ContentPak_BeatSaber)

# Set target properties for client library
set_target_properties(BeatSaberGameClient PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${GAME_HEADERS}"
    OUTPUT_NAME "BeatSaberGameClient"
    # Set the output directory for the library
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    # Export all symbols on Windows for DLL
    WINDOWS_EXPORT_ALL_SYMBOLS ON
)

# For multi-config generators, remove the config subdirectory
foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${CONFIG} CONFIG_UPPER)
    set_target_properties(BeatSaberGameClient PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/libs
        ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/libs
        RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/bin
    )
endforeach()

# Include directories for client
target_include_directories(BeatSaberGameClient
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
        ${CMAKE_SOURCE_DIR}/Engine/Source  # Access to Engine headers
)

# Link libraries for client
target_link_libraries(BeatSaberGameClient
    PUBLIC
        TKDEngineClient  # Link against client engine (with graphics)
        sfml-system
        sfml-window
        sfml-graphics
        sfml-audio
        sfml-network
        OpenGL::GL
        OpenGL::GLU
        GLEW::GLEW
        openvr::openvr
)

# Create server game library
add_library(BeatSaberGameServer SHARED ${GAME_SOURCES})

# Set target properties for server library
set_target_properties(BeatSaberGameServer PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${GAME_HEADERS}"
    OUTPUT_NAME "BeatSaberGameServer"
    # Set the output directory for the library
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    # Export all symbols on Windows for DLL
    WINDOWS_EXPORT_ALL_SYMBOLS ON
)

# For multi-config generators, remove the config subdirectory
foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${CONFIG} CONFIG_UPPER)
    set_target_properties(BeatSaberGameServer PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/libs
        ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/libs
        RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/bin
    )
endforeach()

# Include directories for server
target_include_directories(BeatSaberGameServer
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
        ${CMAKE_SOURCE_DIR}/Engine/Source  # Access to Engine headers
)

# Link libraries for server
target_link_libraries(BeatSaberGameServer
    PUBLIC
        TKDEngineServer  # Link against server engine (no graphics)
)

# Compiler-specific options for client
target_compile_options(BeatSaberGameClient PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
)

# Compiler-specific options for server
target_compile_options(BeatSaberGameServer PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
)

# Create BeatSaberGameClient executable (Client + Game + Main.cpp from Engine)
add_executable(BeatSaberGameClientExe ${CMAKE_SOURCE_DIR}/Engine/Source/Main.cpp)

set_target_properties(BeatSaberGameClientExe PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    OUTPUT_NAME "BeatSaberGameClient"
)

# For multi-config generators, remove the config subdirectory
foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${CONFIG} CONFIG_UPPER)
    set_target_properties(BeatSaberGameClientExe PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/bin
    )
endforeach()

target_include_directories(BeatSaberGameClientExe PRIVATE
    ${CMAKE_SOURCE_DIR}/Engine/Source
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_link_libraries(BeatSaberGameClientExe PRIVATE
    TKDEngineClient
    BeatSaberGameClient
)

# Create BeatSaberGameServer executable (Server + Game + Main.cpp from Engine)
add_executable(BeatSaberGameServerExe ${CMAKE_SOURCE_DIR}/Engine/Source/Main.cpp)

set_target_properties(BeatSaberGameServerExe PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    OUTPUT_NAME "BeatSaberGameServer"
)

# For multi-config generators, remove the config subdirectory
foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${CONFIG} CONFIG_UPPER)
    set_target_properties(BeatSaberGameServerExe PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/bin
    )
endforeach()

target_include_directories(BeatSaberGameServerExe PRIVATE
    ${CMAKE_SOURCE_DIR}/Engine/Source
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_link_libraries(BeatSaberGameServerExe PRIVATE
    TKDEngineServer
    BeatSaberGameServer
)

# Copy resources if they exist
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Assets)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Assets 
         DESTINATION ${CMAKE_BINARY_DIR}/bin)
endif()

# Installation rules
include(GNUInstallDirs)

install(TARGETS BeatSaberGameClient BeatSaberGameServer BeatSaberGameClientExe BeatSaberGameServerExe
    EXPORT BeatSaberGameTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/BeatSaberGame
)
