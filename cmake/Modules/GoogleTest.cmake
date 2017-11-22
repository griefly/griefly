# Props to http://crascit.com/2015/07/25/cmake-gtest/
# Download and unpack googletest at configure time

if(MSVC) # VS2012 doesn't support correctly the tuples yet
    add_definitions(/D _VARIADIC_MAX=10)
endif()

configure_file(cmake/Modules/GoogleTestDownload.cmake
               googletest-download/CMakeLists.txt)
if(CMAKE_BUILD_TYPE MATCHES DEBUG) # Build gtest in release if so.
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
else()
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" -DCMAKE_BUILD_TYPE=Release .
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
    execute_process(COMMAND ${CMAKE_COMMAND} --build . --config Release
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
endif()

# Prevent GoogleTest from overriding our compiler/linker options
# when building with Visual Studio
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This adds
# the following targets: gtest, gtest_main, gmock
# and gmock_main
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
                 ${CMAKE_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

add_definitions(-D _BUILD_TESTS)
