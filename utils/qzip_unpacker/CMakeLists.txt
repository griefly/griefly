message(STATUS "Running qzip_unpacker CMakeLists.txt...")

find_package(Qt5 5.8 COMPONENTS Core REQUIRED)

file(GLOB_RECURSE UNPACKER_SOURCES "*.cpp")
list(APPEND UNPACKER_SOURCES "${COREPATH}/FastSerializer.h")
list(APPEND UNPACKER_SOURCES "${COREPATH}/KvAbort.h")

add_executable(Unpacker ${UNPACKER_SOURCES})

# Add Qt5 lib
qt5_use_modules(Unpacker Core)

message(STATUS "Finished qzip_unpacker CMakeLists.txt")
