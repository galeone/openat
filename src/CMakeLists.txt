cmake_minimum_required (VERSION 3.1)

#include_directories("${INCLUDES}") # from project root CMakeLists.txt

# all files .cc in . and its subfolders.
# define variable CURRENT_SRC
file(GLOB_RECURSE CURRENT_SRC "*.cc")
list(REMOVE_ITEM CURRENT_SRC "main.cc")

# Find threads to link next in target_link_libraries
find_package(Threads REQUIRED)
# Find also OpenSSL
find_package(OpenSSL REQUIRED)

include_directories(
    ${OPENSSL_INCLUDE_DIR}
)

# Executable at is the compilation of every .cc in this folder
# and its subfolders
add_library (at "${CURRENT_SRC}")

target_link_libraries (at LINK_PUBLIC
    curlpp
    ${CMAKE_THREAD_LIBS_INIT}
    gumbo_query_shared #defined in CMakeLists of libs/gumbo/query
    ${OPENSSL_CRYPTO_LIBRARY}
    SQLiteCpp # defined in CMakeLists of libs/SQLiteCpp
)


# Find packages required by main
find_package(Sqlite3 REQUIRED)
# Add "main" target; compile as executable
add_executable (main "main.cc")
include_directories(
    ${INCLUDE}
    ${SQLITE3_INCLUDE_DIRS}
)
target_link_libraries ( main LINK_PUBLIC
    at
    ${SQLITE3_LIBRARIES}
    SQLiteCpp # defined in CMakeLists of libs/SQLiteCpp
)
