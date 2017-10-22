cmake_minimum_required (VERSION 3.1)

# all files .cc in . and its subfolders.
# define variable TEST_SRC
file(GLOB_RECURSE TEST_SRC "*.cc")

# Find threads to link next in target_link_libraries
find_package(Threads REQUIRED)

add_executable (runUnitTests "${TEST_SRC}")
include_directories(
    ${AT_INCLUDE_DIR}
    ${JSON_INCLUDE_DIR}
    ${gtest_SOURCE_DIR}/include
)

target_link_libraries ( runUnitTests LINK_PUBLIC
    at
    gtest
)
