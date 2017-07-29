cmake_minimum_required (VERSION 3.1)

#include_directories("${INCLUDES}") # from project root CMakeLists.txt

# all files .cc in . and its subfolders.
# define variable CURRENT_SRC
file(GLOB_RECURSE CURRENT_SRC "*.cc")

# Find threads to link next in target_link_libraries
find_package(Threads)

# Executable at is the compilation of every .cc file
add_executable (at "${CURRENT_SRC}")

target_link_libraries (at LINK_PUBLIC
    curlpp
    ${CMAKE_THREAD_LIBS_INIT}
    gumbo_query_shared #defined in CMakeLists of libs/gumbo/query
)
