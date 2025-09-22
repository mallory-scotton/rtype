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
if(EXISTS ${CMAKE_BINARY_DIR}/conan_toolchain.cmake)
    include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)
    message(STATUS "Using Conan toolchain")
endif()

if(EXISTS ${CMAKE_BINARY_DIR}/conandeps.cmake)
    include(${CMAKE_BINARY_DIR}/conandeps.cmake)
    message(STATUS "Using Conan dependencies")
endif()

# Add subdirectories
add_subdirectory(Engine)
add_subdirectory(Game)
add_subdirectory(Server)

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
