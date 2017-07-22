# CMakeLists files in this project can
# refer to the root source directory of the project as ${AUTOT_SOURCE_DIR} and
# to the root binary directory of the project as ${AUTOT_BINARY_DIR}.
cmake_minimum_required (VERSION 3.1)
project (at)

set( CMAKE_EXPORT_COMPILE_COMMANDS ON )
# create compile_commands.json in build dir while compiling

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

set (CURLPP "${PROJECT_SOURCE_DIR}/libs/curlpp-0.8.1")
set (JSON "${PROJECT_SOURCE_DIR}/libs/nlohmann")

set (INCLUDES 
  # -I ./include
  ${PROJECT_SOURCE_DIR}/include
  # -I ./libs/nlohmann/ (json.hpp)
  ${JSON}
  # -I ./libs/curpp-0.8-1/include
  ${CURLPP}/include
)

add_subdirectory(${CURLPP})
include_directories("${INCLUDES}")
add_subdirectory(src)

# copy compile commands from build dir to project dir once compiled
ADD_CUSTOM_TARGET(do_always ALL COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
    ${PROJECT_SOURCE_DIR}/compile_commands.json)
