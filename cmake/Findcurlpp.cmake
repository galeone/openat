find_package(PkgConfig)
pkg_check_modules(PC_curlpp QUIET curlpp)

find_path(curlpp_INCLUDE_DIR
    curlpp/Easy.hpp
    PATHS ${PC_curlpp_INCLUDE_DIRS}
    PATH_SUFFIXES curlpp)

find_library(curlpp_SHARED_LIB
    libcurlpp.so
    PATHS ${PC_curlpp_LIBS}
    PATH_SUFFIXES curlpp)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(curlpp REQUIRED_VARS
    curlpp_SHARED_LIB curlpp_INCLUDE_DIR)

if(curlpp_FOUND)
    set(curlpp_INCLUDE_DIRS ${curlpp_INCLUDE_DIR})
endif()

if(curlpp_FOUND AND NOT TARGET libcurlpp::libcurlpp)
    add_library(curlpp::curlpp INTERFACE IMPORTED)
    set_target_properties(curlpp::curlpp PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${curlpp_INCLUDE_DIR}
        INTERFACE_LINK_LIBRARIES ${curlpp_SHARED_LIB}
    )
endif()

