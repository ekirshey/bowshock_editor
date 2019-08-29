#.rst:
# FindGLFW
# --------
#
# Find the OpenGL Extension Wrangler Library (GLFW)
#
# IMPORTED Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines the :prop_tgt:`IMPORTED` target ``GLFW::GLFW``,
# if GLFW has been found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables:
#
# ::
#
#   GLFW_INCLUDE_DIRS - include directories for GLFW
#   GLFW_LIBRARIES - libraries to link against GLFW
#   GLFW_FOUND - true if GLFW has been found and can be used

find_path(GLFW_INCLUDE_DIR GLFW/glfw3.h PATHS ${PROJECT_SOURCE_DIR}/deps/glfw)

if(NOT GLFW_LIBRARY)
  find_library(GLFW_LIBRARY_RELEASE NAMES GLFW3 PATHS ${PROJECT_SOURCE_DIR}/deps/glfw)
  find_library(GLFW_LIBRARY_DEBUG NAMES GLFW3d PATHS ${PROJECT_SOURCE_DIR}/deps/glfw)

  include(${CMAKE_CURRENT_LIST_DIR}/SelectLibraryConfigurations.cmake)
  select_library_configurations(GLFW)
endif ()

include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
find_package_handle_standard_args(GLFW
                                  REQUIRED_VARS GLFW_INCLUDE_DIR GLFW_LIBRARY)

if(GLFW_FOUND)
  set(GLFW_INCLUDE_DIRS ${GLFW_INCLUDE_DIR})

  if(NOT GLFW_LIBRARIES)
    set(GLFW_LIBRARIES ${GLFW_LIBRARY})
  endif()

  if (NOT TARGET GLFW::GLFW)
    add_library(GLFW::GLFW UNKNOWN IMPORTED)
    set_target_properties(GLFW::GLFW PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${GLFW_INCLUDE_DIRS}")

    if(GLFW_LIBRARY_RELEASE)
      set_property(TARGET GLFW::GLFW APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
      set_target_properties(GLFW::GLFW PROPERTIES IMPORTED_LOCATION_RELEASE "${GLFW_LIBRARY_RELEASE}")
    endif()

    if(GLFW_LIBRARY_DEBUG)
      set_property(TARGET GLFW::GLFW APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
      set_target_properties(GLFW::GLFW PROPERTIES IMPORTED_LOCATION_DEBUG "${GLFW_LIBRARY_DEBUG}")
    endif()

    if(NOT GLFW_LIBRARY_RELEASE AND NOT GLFW_LIBRARY_DEBUG)
      set_property(TARGET GLFW::GLFW APPEND PROPERTY IMPORTED_LOCATION "${GLFW_LIBRARY}")
    endif()
  endif()
endif()

mark_as_advanced(GLFW_INCLUDE_DIR)
