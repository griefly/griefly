cmake_minimum_required(VERSION 3.1)
project(KVEngineQt)

option(BUILD_TESTS "Build tests" OFF)
option(BUILD_COVER "Generate test coverage" OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(UTILS_DIR utils/)
set(EXEC_DIR exec/)

set(MINIMUM_QT_VERSION 5.8)

include(ClangFormat)

include(FilterOutList)

# GTEST "search"
if(BUILD_TESTS)
    include(GoogleTest)
endif()

# Some variables neeeded by Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Main game project
add_subdirectory(sources)

# Building launcher
add_subdirectory(utils/launcher)

# Building qzipunpacker
set(SOURCES_PATH "../../sources/")
add_subdirectory(utils/qzip_unpacker)

# Building server
set(GOPATH ${CMAKE_CURRENT_SOURCE_DIR}/gopath)
add_subdirectory(${GOPATH}/src/griefly-server)
