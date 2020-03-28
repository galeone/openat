cmake_minimum_required (VERSION 3.1)
project (openat VERSION 1.0.0 DESCRIPTION "OpenAT: Open Source Algorithmic Trading Library")

#
# BUILD SETTINGS
#

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# Create compile_commands.json in build dir while compiling
set(CMAKE_EXPORT_COMPILE_COMMANDS ON )

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT EXISTS ${CMAKE_BINARY_DIR}/CMakeCache.txt)
  if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
  endif()
endif()

#
# BUILD DEPENDENCIES
#
# Build and setup the correct cmake variables for third-party libraries
#

# Build Curlpp
set(CURLPP "${PROJECT_SOURCE_DIR}/libs/curlpp")
set(CURLPP_INCLUDE_DIR "${CURLPP}/include")
add_subdirectory(${CURLPP})

# Rapidxml is a header-only library
set(RAPIDXML "${PROJECT_SOURCE_DIR}/libs/rapidxml")
set(RAPIDXML_INCLUDE_DIR "${RAPIDXML}")

#
# Build project
#

set(OPENAT_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
add_subdirectory(src)

get_directory_property(hasParent PARENT_DIRECTORY)
# Allows include directories outside this CMake, if it has been included
if(hasParent)
    set( OPENAT_REQUIRED_INCLUDES ${OPENAT_REQUIRED_INCLUDES} PARENT_SCOPE )
endif()

#
# Build tests
#
enable_testing()

# Build googletest
set(GTEST "${PROJECT_SOURCE_DIR}/libs/googletest")
set(GTEST_INCLUDE_DIR "${GTEST}/include")
add_subdirectory(${GTEST})

add_subdirectory(tests)
add_test (NAME openat_tests COMMAND openat_tests)

if(NOT hasParent)
    # copy compile commands from build dir to project dir once compiled
    ADD_CUSTOM_TARGET(openat_do_always ALL COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
        ${PROJECT_SOURCE_DIR}/compile_commands.json)
endif()
