find_package(Go 1.5 REQUIRED)
message(STATUS "Go has been found, version: ${GO_VERSION}")

set(SERVER_DIR ${GOPATH}/src/griefly-server)

file(GLOB_RECURSE GOFILES "${SERVER_DIR}/*.go")


add_custom_command(
    OUTPUT ${SERVER_DIR}/griefly-server${CMAKE_EXECUTABLE_SUFFIX}
    WORKING_DIRECTORY ${SERVER_DIR}
    COMMAND ${CMAKE_COMMAND} -E env GOPATH=${GOPATH} GO15VENDOREXPERIMENT=1 ${GO_EXECUTABLE} build -ldflags "-X main.clientVersionBuild=${CLIENT_VERSION}"
    DEPENDS ${GOFILES})
add_custom_target(GoServer ALL DEPENDS ${SERVER_DIR}/griefly-server${CMAKE_EXECUTABLE_SUFFIX} SOURCES ${GOFILES})
