set(PROJECT_NAME "MapEditor")

message(STATUS "Running ${PROJECT_NAME} CMakeLists.txt...")

set(CMAKE_CXX_STANDARD 14)

find_package(Qt5 ${MINIMUM_QT_VERSION} COMPONENTS Core Widgets REQUIRED)

file(GLOB_RECURSE SOURCES "*.cpp" ".h")

file(GLOB_RECURSE FORMS "*.ui")
qt5_wrap_ui(FORMS_HEADERS ${FORMS})

add_executable(${PROJECT_NAME} ${SOURCES} ${FORMS} ${FORMS_HEADERS})

target_link_libraries(${PROJECT_NAME} Qt5::Core)

install(TARGETS ${PROJECT_NAME}
        DESTINATION "${KV_INSTALL_PATH}")

message(STATUS "Finished ${PROJECT_NAME} CMakeLists.txt")
