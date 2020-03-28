find_path(GUMBO_QUERY_INCLUDE_DIR gq/Document.h)
find_library(GUMBO_QUERY_SHARED_LIB libgq.so)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GumboQuery REQUIRED_VARS
    GUMBO_QUERY_SHARED_LIB GUMBO_QUERY_INCLUDE_DIR)
