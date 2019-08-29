cmake_minimum_required(VERSION 2.8.7 FATAL_ERROR)

project(bowshock_editor)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

# Find needed libraries
# RESTinio dependencies:
# 1. ASIO or Boost::ASIO (goes as headers, vcpkg knows where)
# 2. HTTP parser
find_package(unofficial-http-parser REQUIRED)
# 3. fmtlib
find_package(fmt REQUIRED)
# RESTinio itself
find_package(restinio REQUIRED)

find_package(nlohmann_json CONFIG REQUIRED)


# Find needed libraries
find_package(GLAD)
find_package(GLFW)
find_package(OpenGL)
find_package(Assimp)
find_package(ThirdPartyHeaders)
find_package(Bowshock)

add_subdirectory(src)
