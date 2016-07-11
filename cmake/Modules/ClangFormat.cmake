# Highly experimental/work in progress/etc.
# cmake-tidy requires CMAKE_EXPORT_COMPILE_COMMANDS=ON

file(GLOB_RECURSE ALL_SOURCES "Sources/*.cpp")
file(GLOB_RECURSE ALL_HEADERS "Sources/*.h")

file(GLOB_RECURSE CORE_SOURCES "Sources/core/*.cpp")

find_program(clang-format NAMES clang-format)
find_program(clang-tidy NAMES clang-tidy)

if (clang-format)
    add_custom_target(clang-format SOURCES .clang-format)

    foreach(SOURCE_FILE ${ALL_SOURCES} ${ALL_HEADERS})
      add_custom_command(TARGET clang-format PRE_BUILD
                         COMMAND clang-format -i ${SOURCE_FILE})
    endforeach()
else()
    message(STATUS "No clang-format found")
endif()

if (clang-tidy)
    add_custom_target(clang-tidy SOURCES .clang-tidy)

    if (NOT CMAKE_EXPORT_COMPILE_COMMANDS)
        message(WARNING "CMAKE_EXPORT_COMPILE_COMMANDS is not enabled, clang-tidy probably won't work")
    endif()

    foreach(SOURCE_FILE ${CORE_SOURCES})
        add_custom_command(TARGET clang-tidy COMMAND clang-tidy -enable-check-profile -p=${CMAKE_BINARY_DIR}/compile_commands.json -header-filter=Sources/ -config= ${SOURCE_FILE})
    endforeach()
else()
    message(STATUS "No clang-tidy found")
endif()
