cmake_minimum_required (VERSION 3.1)

# all files .cc in . and its subfolders.
# define variable CURRENT_SRC
file(GLOB_RECURSE CURRENT_SRC "*.cc")

# Find OpenSSL
find_package(OpenSSL REQUIRED)
# sdplog
find_package(spdlog REQUIRED)
# gumbo query
find_package(GumboQuery REQUIRED)
# curlpp
find_package(curlpp REQUIRED)
# JSON
find_package(nlohmann_json REQUIRED)

# Executable at is the compilation of every .cc in this folder
# and its subfolders

add_library (openat SHARED "${CURRENT_SRC}")

target_include_directories(openat
    PUBLIC
        ${OPENAT_INCLUDE_DIR}
        ${RAPIDXML_INCLUDE_DIR}
        ${GUMBO_QUERY_INCLUDE_DIR}
)

target_link_libraries (openat
    PUBLIC
        Threads::Threads
        OpenSSL::SSL
        OpenSSL::Crypto
        curlpp::curlpp
    PRIVATE
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        ${GUMBO_PARSER_SHARED_LIB}
        ${GUMBO_QUERY_SHARED_LIB}
)
