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


# Executable at is the compilation of every .cc in this folder
# and its subfolders
add_library (at "${CURRENT_SRC}")
include_directories(
    ${AT_INCLUDE_DIR}
    ${OPENSSL_INCLUDE_DIR}
    ${JSON_INCLUDE_DIR}
    ${RAPIDXML_INCLUDE_DIR}
    ${SPDLOG_INCLUDE_DIR}
    ${CURLPP_INCLUDE_DIR}
    ${GUMBOPARSER_INCLUDE_DIR}
    ${GUMBOQUERY_INCLUDE_DIR}
)
target_link_libraries (at LINK_PUBLIC
    curlpp
    ${CMAKE_THREAD_LIBS_INIT}
    gumbo_query_shared #defined in CMakeLists of libs/gumbo/query
    ${OPENSSL_CRYPTO_LIBRARY}
)

# Find packages required by main
find_package(Sqlite3 REQUIRED)
# Add "main" target; compile as executable
add_executable (openatd "main.cc")
include_directories(
    ${AT_INCLUDE_DIR}
    ${SQLITE3_INCLUDE_DIRS}
    ${SQLITECPP_INCLUDE_DIR}
)
target_link_libraries (openatd LINK_PUBLIC
    at
    SQLiteCpp # defined in CMakeLists of libs/SQLiteCpp
    ${SQLITE3_LIBRARIES}
    SQLiteCpp # defined in CMakeLists of libs/SQLiteCpp
    ${SQLITE3_LIBRARIES}
)
