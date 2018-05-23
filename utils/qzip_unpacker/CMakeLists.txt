message(STATUS "Running qzip_unpacker CMakeLists.txt...")

set(CMAKE_CXX_STANDARD 14)

find_package(Qt5 ${MINIMUM_QT_VERSION} COMPONENTS Core REQUIRED)

file(GLOB_RECURSE UNPACKER_SOURCES "*.cpp")
list(APPEND UNPACKER_SOURCES "${SOURCES_PATH}/core/FastSerializer.h")
list(APPEND UNPACKER_SOURCES "${SOURCES_PATH}/core/KvAbort.h")

add_executable(Unpacker ${UNPACKER_SOURCES})
target_include_directories(Unpacker PRIVATE "${SOURCES_PATH}")

# Add Qt5 lib
target_link_libraries(Unpacker Qt5::Core)

message(STATUS "Finished qzip_unpacker CMakeLists.txt")
