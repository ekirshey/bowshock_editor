find_path(SPDLOG_INCLUDE_DIR spdlog/spdlog.h PATHS ${PROJECT_SOURCE_DIR}/deps/spdlog/include)

find_path(STB_INCLUDE_DIR stb_image.h PATHS ${PROJECT_SOURCE_DIR}/deps/stb)

find_path(GLM_INCLUDE_DIR glm/glm.hpp PATHS ${PROJECT_SOURCE_DIR}/deps/glm)

mark_as_advanced(GLM_INCLUDE_DIR)
mark_as_advanced(SPDLOG_INCLUDE_DIR)
mark_as_advanced(STB_INCLUDE_DIR)
