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

# Set compilation flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

#
# BUILD DEPENDENCIES
#
# Build and setup the correct cmake variables for third-party libraries
#

# Enable ExternalProject CMake module
include(ExternalProject)

# Build Curlpp
set(CURLPP "${PROJECT_SOURCE_DIR}/libs/curlpp")
set(CURLPP_INCLUDE_DIR "${CURLPP}/include")
add_subdirectory(${CURLPP})

# Json is a header-only library
set(JSON "${PROJECT_SOURCE_DIR}/libs/nlohmann")
set(JSON_INCLUDE_DIR "${JSON}")

# Spdlog is a header-only library
set(SPDLOG "${PROJECT_SOURCE_DIR}/libs/spdlog")
set(SPDLOG_INCLUDE_DIR "${SPDLOG}/include")

# Rapidxml is a header-only library
set(RAPIDXML "${PROJECT_SOURCE_DIR}/libs/rapidxml")
set(RAPIDXML_INCLUDE_DIR "${RAPIDXML}")

# GumboParser uses autogen
set(GUMBOPARSER "${PROJECT_SOURCE_DIR}/libs/gumbo/parser")
set(GUMBOPARSER_INCLUDE_DIR "${GUMBOPARSER}/include")
# The compilation creates the include dir ${GUMBOPARSER}/include
# and the lib dir ${GUMBOPARSER}/lib
ExternalProject_Add(gumbo_parser
    SOURCE_DIR ${GUMBOPARSER}
    # Compile gumbo-parser
    CONFIGURE_COMMAND ${GUMBOPARSER}/autogen.sh COMMAND ./configure --prefix=${GUMBOPARSER}
    PREFIX ${GUMBOPARSER}
    BUILD_COMMAND ${MAKE}
    BUILD_IN_SOURCE 1)


# Build gumbo-query that uses gumbo and cmake
set(GUMBOQUERY "${PROJECT_SOURCE_DIR}/libs/gumbo/query")
set(GUMBOQUERY_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/libs/gumbo") # keep structured includes
# set variables needed by gumboquery cmakelists
set(Gumbo_INCLUDE_DIR "${GUMBOPARSER_INCLUDE_DIR}")
set(Gumbo_static_LIBRARY "${GUMBOPARSER}/lib/libgumbo.a")
set(Gumbo_LIBRARY "${GUMBOPARSER}/lib/libgumbo.so")
if(APPLE)
    set (Gumbo_LIBRARY "${GUMBOPARSER}/lib/libgumbo.dylib")
endif()
add_subdirectory(${GUMBOQUERY})

#
# Build project
#

set(OPENAT_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
add_subdirectory(src)
# Allows include directories outside this CMake
set( OPENAT_REQUIRED_INCLUDES ${OPENAT_REQUIRED_INCLUDES} PARENT_SCOPE )

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

get_directory_property(hasParent PARENT_DIRECTORY)
if(NOT hasParent)
    # copy compile commands from build dir to project dir once compiled
    ADD_CUSTOM_TARGET(openat_do_always ALL COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
        ${PROJECT_SOURCE_DIR}/compile_commands.json)
endif()
