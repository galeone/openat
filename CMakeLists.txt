# CMakeLists files in this project can
# refer to the root source directory of the project as ${AUTOT_SOURCE_DIR} and
# to the root binary directory of the project as ${AUTOT_BINARY_DIR}.
cmake_minimum_required (VERSION 3.1)
project (at)

set ( CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" )

set( CMAKE_EXPORT_COMPILE_COMMANDS ON )
# create compile_commands.json in build dir while compiling

set (CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

set (CURLPP "${PROJECT_SOURCE_DIR}/libs/curlpp-0.8.1")
set (JSON "${PROJECT_SOURCE_DIR}/libs/nlohmann")
set (SPDLOG "${PROJECT_SOURCE_DIR}/libs/spdlog-0.14.0")
set (GUMBO "${PROJECT_SOURCE_DIR}/libs/gumbo")
set (GUMBOPARSER "${GUMBO}/parser")
set (GUMBOQUERY "${GUMBO}/query")
set (SQLITEPP "${PROJECT_SOURCE_DIR}/libs/SQLiteCpp-2.1.0")

# Compile curlpp, that uses cmake
add_subdirectory(${CURLPP})

# Compile gumbo-parser, that uses autogen
include(ExternalProject)
ExternalProject_Add(gumbo_parser
    SOURCE_DIR ${GUMBOPARSER}
    # Compile gumbo-parser
    CONFIGURE_COMMAND ${GUMBOPARSER}/autogen.sh COMMAND ./configure --prefix=${GUMBOPARSER}
    PREFIX ${GUMBOPARSER}
    BUILD_COMMAND ${MAKE}
    BUILD_IN_SOURCE 1)
# The compilation creates the include dir ${GUMBOPARSER}/include
# and the lib dir ${GUMBOPARSER}/lib

# Compile gumbo-query that users gumbo and cmake
set (Gumbo_INCLUDE_DIR "${GUMBOPARSER}/include")
set (Gumbo_static_LIBRARY "${GUMBOPARSER}/lib/libgumbo.a")
set (Gumbo_LIBRARY "${GUMBOPARSER}/lib/libgumbo.so")
if(APPLE)
    set (Gumbo_LIBRARY "${GUMBOPARSER}/lib/libgumbo.dylib")
endif()
add_subdirectory(${GUMBOQUERY}) #defines gumbo_query

# Compile and define SQLiteCpp
add_subdirectory(${SQLITEPP})

set (INCLUDES 
  # -I ./include
  ${PROJECT_SOURCE_DIR}/include
  # -I ./libs/nlohmann/ (json.hpp)
  ${JSON}
  # -I ./libs/spdlog-0.14.0/include
  ${SPDLOG}/include
  # -I ./libs/curpp-0.8-1/include
  ${CURLPP}/include
  # -I ./libs/gumbo/parser/include
  ${GUMBOPARSER}/include
  # -I ./libs/gumbo/ #because gumbo-query is badly structured
  # Instead of having "File.h" included, I prefer to include the whole structure
  # query/src/File.h
  ${GUMBO}/
  # -I /libs/SQLiteCpp-2.1.0/include
  ${SQLITEPP}/include
)

include_directories("${INCLUDES}")
add_subdirectory(src)

# copy compile commands from build dir to project dir once compiled
ADD_CUSTOM_TARGET(do_always ALL COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
    ${PROJECT_SOURCE_DIR}/compile_commands.json)
