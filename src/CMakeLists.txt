cmake_minimum_required (VERSION 3.1)

# all files .cc in . and its subfolders.
# define variable CURRENT_SRC
file(GLOB_RECURSE CURRENT_SRC "*.cc")

# Find OpenSSL
find_package(OpenSSL REQUIRED)

# Executable at is the compilation of every .cc in this folder
# and its subfolders

add_library (openat SHARED "${CURRENT_SRC}")
set(OPENAT_REQUIRED_INCLUDES
    ${OPENAT_INCLUDE_DIR}
    ${OPENSSL_INCLUDE_DIR}
    ${JSON_INCLUDE_DIR}
    ${RAPIDXML_INCLUDE_DIR}
    ${SPDLOG_INCLUDE_DIR}
    ${CURLPP_INCLUDE_DIR}
    ${GUMBOPARSER_INCLUDE_DIR}
    ${GUMBOQUERY_INCLUDE_DIR}
)
include_directories(${OPENAT_REQUIRED_INCLUDES})
# Allows include directories outside this CMake
set( OPENAT_REQUIRED_INCLUDES ${OPENAT_REQUIRED_INCLUDES} PARENT_SCOPE )

target_link_libraries (openat LINK_PUBLIC
    curlpp
    ${CMAKE_THREAD_LIBS_INIT}
    gumbo_query_shared #defined in CMakeLists of libs/gumbo/query
    ${OPENSSL_CRYPTO_LIBRARY}
)

include(GNUInstallDirs)
configure_file("${PROJECT_SOURCE_DIR}/openat.pc.in" openat.pc @ONLY)
install(TARGETS openat
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${CMAKE_BINARY_DIR}/src/openat.pc
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)
