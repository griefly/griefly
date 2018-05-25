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

########################################
#
# Some global compiler flags & other stuff
#
########################################

set(CMAKE_CXX_STANDARD 14)
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-Werror -Wold-style-cast -Wreturn-type ${CMAKE_CXX_FLAGS}")
endif()

set(CLIENT_DIR "./")
file(GLOB_RECURSE CLIENT_SOURCES "${CLIENT_DIR}*.cpp" "${CLIENT_DIR}*.h")
file(GLOB_RECURSE FORMS "${CLIENT_DIR}*.ui")

qt5_wrap_ui(FORMS_HEADERS ${FORMS})
add_executable(KVClient WIN32 ${CLIENT_SOURCES} ${FORMS} ${FORMS_HEADERS})

target_link_libraries(KVClient KVEngine)

target_link_libraries(KVClient Qt5::Core Qt5::Network Qt5::Widgets Qt5::OpenGL Qt5::Multimedia)

# Add opengl lib
if(WIN32)
    target_link_libraries(KVClient opengl32)
elseif(APPLE)
    target_link_libraries(KVClient /System/Library/Frameworks/OpenGL.framework)
else()
    target_link_libraries(KVClient GL)
endif()

# Add coverage support
if(BUILD_COVER)
    if(CMAKE_COMPILER_IS_GNUCXX)
        target_link_libraries(KVClient gcov)
    endif()
endif()

install(TARGETS KVClient
        DESTINATION "${KV_INSTALL_PATH}")

