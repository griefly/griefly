find_package(Qt5 ${MINIMUM_QT_VERSION} COMPONENTS Core Widgets REQUIRED)

file(GLOB_RECURSE LAUNCHER_SOURCES "*.cpp")
file(GLOB_RECURSE LAUNCHER_HEADERS "*.h")
file(GLOB_RECURSE LAUNCHER_FORMS "*.ui")

qt5_wrap_ui(LAUNCHER_FORMS_HEADERS ${LAUNCHER_FORMS})

add_executable(Launcher WIN32 ${LAUNCHER_SOURCES} ${LAUNCHER_HEADERS} ${LAUNCHER_FORMS_HEADERS})

# Add Qt5 lib
target_link_libraries(Launcher Qt5::Core Qt5::Widgets)

if(WIN32)
    target_link_libraries(Launcher Qt5::WinMain)
endif()

install(TARGETS Launcher
        DESTINATION "${KV_INSTALL_PATH}")
