cmake_minimum_required (VERSION 3.1)

# all files .cc in . and its subfolders.
# define variable TEST_SRC
file(GLOB_RECURSE TEST_SRC "*.cc")

# Find threads to link next in target_link_libraries
find_package(Threads REQUIRED)

add_executable (openat_tests "${TEST_SRC}")
include_directories(
    ${OPENAT_INCLUDE_DIR}
    ${JSON_INCLUDE_DIR}
    ${GTEST_INCLUDE_DIR}
)

target_link_libraries ( openat_tests LINK_PUBLIC
    openat
    gtest
)
