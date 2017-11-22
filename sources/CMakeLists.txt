##################################
### Start search dependencies
###

###########################
#
# Git search
#
###########################
find_package(Git REQUIRED)
message(STATUS "Git has been found, ${GIT_VERSION_STRING}")

###########################
#
# Qt5 search
#
###########################
find_package(Qt5 ${MINIMUM_QT_VERSION} COMPONENTS Core Network Widgets OpenGL Multimedia REQUIRED)

message(STATUS "Qt5 has been found: ${Qt5_VERSION}")

get_target_property(QtCore_location Qt5::Core LOCATION)
message(STATUS "Core path: ${QtCore_location}")

get_target_property(QtNetwork_location Qt5::Network LOCATION)
message(STATUS "Network path: ${QtNetwork_location}")

get_target_property(QtWidgets_location Qt5::Widgets LOCATION)
message(STATUS "Widgets path: ${QtWidgets_location}")

get_target_property(QtOpenGL_location Qt5::OpenGL LOCATION)
message(STATUS "OpenGL path: ${QtOpenGL_location}")

get_target_property(QtMultimedia_location Qt5::Multimedia LOCATION)
message(STATUS "Multimedia path: ${QtMultimedia_location}")

###########################
#
# Python 3.5 search
#
###########################
find_package(PythonInterp 3.5 REQUIRED)
message(STATUS "Python has been found, version: ${PYTHON_VERSION_STRING}")

###
### End search dependecies
#######################################

#######################################
### Checking build version
###

execute_process(
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND ${GIT_EXECUTABLE} rev-parse --git-dir
    OUTPUT_VARIABLE IS_GIT_REPO OUTPUT_STRIP_TRAILING_WHITESPACE)
if(IS_GIT_REPO)
    message(STATUS "Source dir is active git repo")
else()
    message(FATAL_ERROR "Source dir is not active git repo: ${CMAKE_SOURCE_DIR}")
endif()

execute_process(
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND ${GIT_EXECUTABLE} describe --tags --abbrev=4
    OUTPUT_VARIABLE CLIENT_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
if(NOT CLIENT_VERSION)
    # No tags or shallow copy
    set(CLIENT_VERSION "v0.0.0")
endif()
message(STATUS ${CLIENT_VERSION})

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(BITS_VERSION 64)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(BITS_VERSION 32)
else()
    message(FATAL_ERROR "Unsupported compiler bits version!")
endif()
set(SYSTEM_INFO "\
System: ${CMAKE_SYSTEM}, \
Compiler: ${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}, \
Bits version: ${BITS_VERSION}\
")
message(STATUS ${SYSTEM_INFO})

# Version.cpp generation
execute_process(
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/${UTILS_DIR}/version.py
    ${CLIENT_VERSION} ${SYSTEM_INFO}
    RESULT_VARIABLE VERSION_GENERATION_RETVAL
)
if(NOT VERSION_GENERATION_RETVAL EQUAL 0)
    message(FATAL_ERROR "Unable to generate 'Version.cpp' file!")
endif()

###
### End checking build version
##########################################



###########################################
### Some global compiler flags & other stuff
###

set(CMAKE_CXX_STANDARD 14)
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-Werror -Wold-style-cast -Wreturn-type ${CMAKE_CXX_FLAGS}")
endif()

#SET(CMAKE_EXE_LINKER_FLAGS "/DEBUG")

###
### Global compiler glags have been setted
##########################################



##################################
### Building client
###

set(SOURCE_DIR ./)
set(TESTS_DIR tests/)

# include_directories(SOURCE_DIR)
# Workaround for linux
include_directories(".")

if(BUILD_TESTS)
    file(GLOB_RECURSE TESTS "${TESTS_DIR}*.cpp" "${TESTS_DIR}*.h")
else()
    set(TESTS "")
endif()

file(GLOB_RECURSE SOURCES "${SOURCE_DIR}*.cpp")
file(GLOB_RECURSE HEADERS "${SOURCE_DIR}*.h")
file(GLOB_RECURSE FORMS "${SOURCE_DIR}*.ui")

# Metadata & source code generation
add_custom_command(
    OUTPUT metadata.json
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/${UTILS_DIR}/metadata_generator.py ${CMAKE_CURRENT_SOURCE_DIR} metadata.json
    DEPENDS ${HEADERS})
add_custom_command(
    OUTPUT AutogenMetadata.cpp AutogenSerialization.cpp
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/${UTILS_DIR}/autogen.py
    DEPENDS metadata.json
)
add_custom_target(Metadata ALL DEPENDS AutogenMetadata.cpp AutogenSerialization.cpp)

list(APPEND SOURCES "${CMAKE_CURRENT_BINARY_DIR}/AutogenMetadata.cpp")
list(APPEND SOURCES "${CMAKE_CURRENT_BINARY_DIR}/AutogenSerialization.cpp")
list(APPEND SOURCES "${CMAKE_CURRENT_BINARY_DIR}/Version.cpp")

qt5_wrap_ui(FORMS_HEADERS ${FORMS})

add_executable(KVEngine WIN32 ${SOURCES} ${HEADERS} ${FORMS_HEADERS} ${TESTS})

# Add Qt5 lib
qt5_use_modules(KVEngine Core)
qt5_use_modules(KVEngine Network)
qt5_use_modules(KVEngine Widgets)
qt5_use_modules(KVEngine OpenGL)
qt5_use_modules(KVEngine Multimedia)

# Add GTEST lib
if(BUILD_TESTS)
    include_directories("${gtest_SOURCE_DIR}/include"
                        "${gmock_SOURCE_DIR}/include")
    target_link_libraries(KVEngine gtest)
    target_link_libraries(KVEngine gmock)
    if(MSVC)
        set_target_properties(KVEngine PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:CONSOLE")
        set_target_properties(KVEngine PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
    endif()
else()
    if(MSVC)
        target_link_libraries(KVEngine Qt5::WinMain)
    endif()
endif()

# Add opengl lib
if(WIN32)
    target_link_libraries(KVEngine opengl32)
elseif(APPLE)
    target_link_libraries(KVEngine /System/Library/Frameworks/OpenGL.framework)
else()
    target_link_libraries(KVEngine GL)
endif()

# Add coverage support
if(BUILD_COVER)
    if(CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0") # debug, no optimisation
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage") # enabling coverage
        add_definitions(-D _BUILD_COVER)
    else()
        message(FATAL_ERROR "Tests coverage is not supported for Windows!")
    endif()
endif()

###
### End building client
##################################

# Install rules

install(TARGETS KVEngine
        DESTINATION ${CMAKE_SOURCE_DIR}/${EXEC_DIR}/)

install(FILES ${CMAKE_BINARY_DIR}/metadata.json
        DESTINATION ${CMAKE_SOURCE_DIR}/${EXEC_DIR}/)
