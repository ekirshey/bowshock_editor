
find_path(GLAD_INCLUDE_DIR glad/glad.h PATHS ${PROJECT_SOURCE_DIR}/deps/glad/include)
find_path(KHR_INCLUDE_DIR KHR/khrplatform.h PATHS ${PROJECT_SOURCE_DIR}/deps/glad/include)
find_path(GLAD_SOURCE_DIR glad.c PATHS ${PROJECT_SOURCE_DIR}/deps/glad/src)


mark_as_advanced(GLAD_INCLUDE_DIR)
mark_as_advanced(KHR_INCLUDE_DIR)
mark_as_advanced(GLAD_SOURCE_DIR)
