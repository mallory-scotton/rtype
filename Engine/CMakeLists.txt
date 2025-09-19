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

# Options
option(BUILD_TESTS "Build unit tests" ON)

# Conan integration
if(EXISTS ${CMAKE_BINARY_DIR}/conan_toolchain.cmake)
    include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)
endif()

if(EXISTS ${CMAKE_BINARY_DIR}/conandeps.cmake)
    include(${CMAKE_BINARY_DIR}/conandeps.cmake)
endif()

# Find dependencies
find_package(SFML 2.6.1 REQUIRED COMPONENTS system window graphics audio network)

# Find test dependencies if building tests
if(BUILD_TESTS)
    find_package(Catch2 REQUIRED)
    enable_testing()
endif()

# Collect source files
file(GLOB_RECURSE ENGINE_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.hpp"
)

file(GLOB_RECURSE ENGINE_HEADERS 
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h"
)

# Create shared library for client (with graphics)
add_library(RTypeEngineClient SHARED ${ENGINE_SOURCES})

# Create shared library for server (without graphics)
add_library(RTypeEngineServer SHARED ${ENGINE_SOURCES})

# Set target properties for client
set_target_properties(RTypeEngineClient PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${ENGINE_HEADERS}"
    OUTPUT_NAME "RTypeEngineClient"
    # Set the output directory for the library
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Set target properties for server
set_target_properties(RTypeEngineServer PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${ENGINE_HEADERS}"
    OUTPUT_NAME "RTypeEngineServer"
    # Set the output directory for the library
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Include directories for client
target_include_directories(RTypeEngineClient
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

# Include directories for server
target_include_directories(RTypeEngineServer
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

# Link libraries for client (with all SFML components)
target_link_libraries(RTypeEngineClient
    PUBLIC
        sfml-system
        sfml-window
        sfml-graphics
        sfml-audio
        sfml-network
)

# Link libraries for server (minimal SFML components)
target_link_libraries(RTypeEngineServer
    PUBLIC
        sfml-system
        sfml-network
)

# Define graphics support for client
target_compile_definitions(RTypeEngineClient PUBLIC
    TKD_ENGINE_WITH_GRAPHICS
)

# Compiler-specific options for client
target_compile_options(RTypeEngineClient PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
)

# Compiler-specific options for server
target_compile_options(RTypeEngineServer PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
)

# Export symbols for Windows
if(WIN32)
    target_compile_definitions(RTypeEngineClient PRIVATE
        ENGINE_EXPORTS
    )
    target_compile_definitions(RTypeEngineClient PUBLIC
        $<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:SFML_STATIC>
    )
    target_compile_definitions(RTypeEngineServer PRIVATE
        ENGINE_EXPORTS
    )
    target_compile_definitions(RTypeEngineServer PUBLIC
        $<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:SFML_STATIC>
    )
endif()

# Create lib directory if it doesn't exist
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Add custom command to ensure libraries are copied to Build/lib
add_custom_command(TARGET RTypeEngineClient POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/lib
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:RTypeEngineClient> ${CMAKE_BINARY_DIR}/lib/
    COMMENT "Copying RTypeEngineClient library to Build/lib directory"
)

add_custom_command(TARGET RTypeEngineServer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/lib
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:RTypeEngineServer> ${CMAKE_BINARY_DIR}/lib/
    COMMENT "Copying RTypeEngineServer library to Build/lib directory"
)

# Installation rules
include(GNUInstallDirs)

install(TARGETS RTypeEngineClient RTypeEngineServer
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

# Build tests if enabled
if(BUILD_TESTS)
    # Collect test source files
    file(GLOB_RECURSE TEST_SOURCES 
        "${CMAKE_CURRENT_SOURCE_DIR}/Tests/*.cpp"
    )

    # Create test executable
    add_executable(RTypeEngineTests ${TEST_SOURCES})

    # Set target properties
    set_target_properties(RTypeEngineTests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )

    # Include directories for tests
    target_include_directories(RTypeEngineTests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
        ${CMAKE_CURRENT_SOURCE_DIR}/Tests
    )

    # Link libraries for tests (use client version for full functionality)
    target_link_libraries(RTypeEngineTests PRIVATE
        RTypeEngineClient
        Catch2::Catch2
    )

    # Compiler-specific options for tests
    target_compile_options(RTypeEngineTests PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
        $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
    )

    # Register test with CTest
    include(CTest)
    if(TARGET Catch2::Catch2)
        # Catch2 v2 style
        include(${CONAN_CATCH2_ROOT}/lib/cmake/Catch2/Catch.cmake OPTIONAL)
        if(COMMAND catch_discover_tests)
            catch_discover_tests(RTypeEngineTests)
        else()
            add_test(NAME RTypeEngineTests COMMAND RTypeEngineTests)
        endif()
    else()
        # Fallback to simple test registration
        add_test(NAME RTypeEngineTests COMMAND RTypeEngineTests)
    endif()
endif()
