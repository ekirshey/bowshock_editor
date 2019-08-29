# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables:
#
# ::
#
#   BOWSHOCK_INCLUDE_DIRS - include directories for BOWSHOCK
#   BOWSHOCK_LIBRARIES - libraries to link against BOWSHOCK
#   BOWSHOCK_FOUND - true if BOWSHOCK has been found and can be used

find_path(BOWSHOCK_INCLUDE_DIR bowshock/model.h PATHS ${PROJECT_SOURCE_DIR}/deps/bowshock)

find_library(BOWSHOCK_LIBRARY_RELEASE NAMES bowshock PATHS ${PROJECT_SOURCE_DIR}/deps/bowshock/lib)

include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
find_package_handle_standard_args(bowshock
                                  REQUIRED_VARS BOWSHOCK_INCLUDE_DIR BOWSHOCK_LIBRARY_RELEASE)

mark_as_advanced(BOWSHOCK_LIBRARY_RELEASE)
mark_as_advanced(BOWSHOCK_INCLUDE_DIR)
