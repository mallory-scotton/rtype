cmake_minimum_required(VERSION 3.21)

project(RType VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard globally
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Conan integration for the entire project
if(EXISTS ${CMAKE_BINARY_DIR}/build/Release/generators/conan_toolchain.cmake)
    include(${CMAKE_BINARY_DIR}/build/Release/generators/conan_toolchain.cmake)
    message(STATUS "Using Conan toolchain")
endif()

if(EXISTS ${CMAKE_BINARY_DIR}/build/Release/generators/conandeps_legacy.cmake)
    include(${CMAKE_BINARY_DIR}/build/Release/generators/conandeps_legacy.cmake)
    message(STATUS "Using Conan dependencies")
endif()

# Create output directories
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/libs)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add subdirectories
add_subdirectory(Engine)
add_subdirectory(Tools/TKDPack)
add_subdirectory(Tools/TKDEmbed)
add_subdirectory(Games/RType)
add_subdirectory(Games/BeatSaber)

# Clang-Format target
find_program(CLANG_FORMAT "clang-format")
if(CLANG_FORMAT)
    file(GLOB_RECURSE ALL_CXX_SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/*.cpp
        ${CMAKE_SOURCE_DIR}/*.h
    )
    
    add_custom_target(
        format
        COMMAND ${CLANG_FORMAT}
        -i
        ${ALL_CXX_SOURCE_FILES}
    )
endif()
