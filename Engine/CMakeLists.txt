cmake_minimum_required(VERSION 3.21)

project(TKDEngine VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Enable position independent code for shared library
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Options
option(BUILD_TESTS "Build unit tests" ON)

# Conan integration
if(EXISTS ${CMAKE_BINARY_DIR}/build/Release/generators/conan_toolchain.cmake)
    include(${CMAKE_BINARY_DIR}/build/Release/generators/conan_toolchain.cmake)
endif()

if(EXISTS ${CMAKE_BINARY_DIR}/build/Release/generators/conandeps_legacy.cmake)
    include(${CMAKE_BINARY_DIR}/build/Release/generators/conandeps_legacy.cmake)
endif()

# Find dependencies
find_package(SFML 2.6.1 REQUIRED COMPONENTS system window graphics audio network)
find_package(imgui REQUIRED)
find_package(ImGui-SFML REQUIRED)
find_package(asio REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenVR REQUIRED)
find_package(nlohmann_json REQUIRED)

# Find test dependencies if building tests
if(BUILD_TESTS)
    find_package(Catch2 REQUIRED)
    enable_testing()
endif()

# Collect source files (excluding Main.cpp for libraries)
file(GLOB_RECURSE ENGINE_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.hpp"
)

# Remove Main.cpp from library sources
list(FILTER ENGINE_SOURCES EXCLUDE REGEX ".*Main\\.cpp$")

file(GLOB_RECURSE ENGINE_HEADERS 
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h"
)

# Create shared library for client (with graphics)
add_library(TKDEngineClient SHARED ${ENGINE_SOURCES})

# Create shared library for server (without graphics)
add_library(TKDEngineServer SHARED ${ENGINE_SOURCES})

# Set target properties for client
set_target_properties(TKDEngineClient PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${ENGINE_HEADERS}"
    OUTPUT_NAME "TKDEngineClient"
    # Set the output directory for the library
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Set target properties for server
set_target_properties(TKDEngineServer PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${ENGINE_HEADERS}"
    OUTPUT_NAME "TKDEngineServer"
    # Set the output directory for the library
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Include directories for client
target_include_directories(TKDEngineClient
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

# Include directories for server
target_include_directories(TKDEngineServer
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

# Link libraries for client (with all SFML components)
target_link_libraries(TKDEngineClient
    PUBLIC
        sfml::sfml
        asio::asio
        imgui::imgui
        ImGui-SFML::ImGui-SFML
        GLEW::GLEW
        openvr::openvr
        GLU
        GL
        nlohmann_json::nlohmann_json
)

# Link libraries for server (minimal SFML components)
target_link_libraries(TKDEngineServer
    PUBLIC
        sfml::sfml
        asio::asio
        nlohmann_json::nlohmann_json
)

# Define graphics support for client
target_compile_definitions(TKDEngineClient PUBLIC
    TKD_ENGINE_WITH_GRAPHICS
)

# Compiler-specific options for client
target_compile_options(TKDEngineClient PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
)

# Compiler-specific options for server
target_compile_options(TKDEngineServer PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
)

# Export symbols for Windows
if(WIN32)
    target_compile_definitions(TKDEngineClient PRIVATE
        ENGINE_EXPORTS
    )
    target_compile_definitions(TKDEngineClient PUBLIC
        $<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:SFML_STATIC>
    )
    target_compile_definitions(TKDEngineServer PRIVATE
        ENGINE_EXPORTS
    )
    target_compile_definitions(TKDEngineServer PUBLIC
        $<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:SFML_STATIC>
    )
endif()

# Create libs directory if it doesn't exist
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/libs)

# Add custom command to ensure libraries are copied to Build/libs
add_custom_command(TARGET TKDEngineClient POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/libs
    COMMENT "Ensuring TKDEngineClient library directory exists"
)

add_custom_command(TARGET TKDEngineServer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/libs
    COMMENT "Ensuring TKDEngineServer library directory exists"
)

# Create TKDEngineClient executable
add_executable(TKDEngineClientExe ${CMAKE_CURRENT_SOURCE_DIR}/Source/Main.cpp)

set_target_properties(TKDEngineClientExe PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    OUTPUT_NAME "TKDEngineClient"
)

target_include_directories(TKDEngineClientExe PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_link_libraries(TKDEngineClientExe PRIVATE
    TKDEngineClient
)

# Create TKDEngineServer executable
add_executable(TKDEngineServerExe ${CMAKE_CURRENT_SOURCE_DIR}/Source/Main.cpp)

set_target_properties(TKDEngineServerExe PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    OUTPUT_NAME "TKDEngineServer"
)

target_include_directories(TKDEngineServerExe PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_link_libraries(TKDEngineServerExe PRIVATE
    TKDEngineServer
)

# Installation rules
include(GNUInstallDirs)

install(TARGETS TKDEngineClient TKDEngineServer
    EXPORT TKDEngineTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/TKDEngine
)

# Install export file
install(EXPORT TKDEngineTargets
    FILE TKDEngineTargets.cmake
    NAMESPACE TKD::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/TKDEngine
)

# Create config file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    TKDEngineConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/TKDEngineConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/TKDEngineConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/TKDEngine
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/TKDEngineConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/TKDEngineConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/TKDEngine
)

# Build tests if enabled
if(BUILD_TESTS)
    # Collect test source files
    file(GLOB_RECURSE TEST_SOURCES 
        "${CMAKE_CURRENT_SOURCE_DIR}/Tests/*.cpp"
    )

    # Create test executable
    add_executable(TKDEngineTests ${TEST_SOURCES})

    # Set target properties
    set_target_properties(TKDEngineTests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )

    # Include directories for tests
    target_include_directories(TKDEngineTests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
        ${CMAKE_CURRENT_SOURCE_DIR}/Tests
    )

    # Link libraries for tests (use client version for full functionality)
    target_link_libraries(TKDEngineTests PRIVATE
        TKDEngineClient
        Catch2::Catch2WithMain
    )

    # Compiler-specific options for tests
    target_compile_options(TKDEngineTests PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
        $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
    )

    # Register test with CTest
    include(CTest)
    # Use simple test registration to avoid test discovery issues during build
    add_test(NAME TKDEngineTests COMMAND TKDEngineTests)
endif()
