set(PROJECT_NAME "basic_assets_generator")

message(STATUS "Running ${PROJECT_NAME} CMakeLists.txt...")

set(CMAKE_CXX_STANDARD 14)

find_package(Qt5 ${MINIMUM_QT_VERSION} COMPONENTS Core REQUIRED)

file(GLOB_RECURSE SOURCES "*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} KVEngine)
target_link_libraries(${PROJECT_NAME} Qt5::Core)

message(STATUS "Finished ${PROJECT_NAME} CMakeLists.txt")
