cmake_minimum_required(VERSION 3.21)

project(RTypeEngine VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Enable position independent code for shared library
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Conan integration
if(EXISTS ${CMAKE_BINARY_DIR}/conan_toolchain.cmake)
    include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)
endif()

if(EXISTS ${CMAKE_BINARY_DIR}/conandeps.cmake)
    include(${CMAKE_BINARY_DIR}/conandeps.cmake)
endif()

# Find dependencies
find_package(SFML 2.6.1 REQUIRED COMPONENTS system window graphics audio network)

# Collect source files
file(GLOB_RECURSE ENGINE_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.hpp"
)

file(GLOB_RECURSE ENGINE_HEADERS 
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h"
)

# Create shared library
add_library(RTypeEngine SHARED ${ENGINE_SOURCES})

# Set target properties
set_target_properties(RTypeEngine PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${ENGINE_HEADERS}"
)

# Include directories
target_include_directories(RTypeEngine
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

# Link libraries
target_link_libraries(RTypeEngine
    PUBLIC
        sfml-system
        sfml-window
        sfml-graphics
        sfml-audio
        sfml-network
)

# Compiler-specific options
target_compile_options(RTypeEngine PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
)

# Export symbols for Windows
if(WIN32)
    target_compile_definitions(RTypeEngine PRIVATE
        ENGINE_EXPORTS
    )
    target_compile_definitions(RTypeEngine PUBLIC
        $<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:SFML_STATIC>
    )
endif()

# Installation rules
include(GNUInstallDirs)

install(TARGETS RTypeEngine
    EXPORT RTypeEngineTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/RTypeEngine
)

# Install export file
install(EXPORT RTypeEngineTargets
    FILE RTypeEngineTargets.cmake
    NAMESPACE RType::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/RTypeEngine
)

# Create config file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    RTypeEngineConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/RTypeEngineConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/RTypeEngineConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/RTypeEngine
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/RTypeEngineConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/RTypeEngineConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/RTypeEngine
)
