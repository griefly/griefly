# Highly experimental, don't use those targets unless the official guide says so
# cmake-tidy requires CMAKE_EXPORT_COMPILE_COMMANDS=ON

file(GLOB_RECURSE ALL_SOURCES "sources/*.cpp")
file(GLOB_RECURSE ALL_HEADERS "sources/*.h")

find_program(clang-format NAMES clang-format clang-format-4.0)
find_program(clang-tidy NAMES clang-tidy)

if (clang-format)
    message(STATUS "Clang format has been found: ${clang-format}")
    add_custom_target(clang-format SOURCES .clang-format)
    set_target_properties(clang-format PROPERTIES EXCLUDE_FROM_ALL TRUE EXCLUDE_FROM_DEFAULT_BUILD TRUE)

    foreach(SOURCE_FILE ${ALL_SOURCES} ${ALL_HEADERS})
      add_custom_command(TARGET clang-format PRE_BUILD
                         COMMAND ${clang-format} -i ${SOURCE_FILE})
    endforeach()
else()
    message(STATUS "No clang-format found")
endif()

if (clang-tidy)
    message(STATUS "Clang tidy has been found: ${clang-tidy}")
    add_custom_target(clang-tidy SOURCES .clang-tidy)
    set_target_properties(clang-tidy PROPERTIES EXCLUDE_FROM_ALL TRUE EXCLUDE_FROM_DEFAULT_BUILD TRUE)

    if (NOT CMAKE_EXPORT_COMPILE_COMMANDS)
        message(WARNING "CMAKE_EXPORT_COMPILE_COMMANDS is not enabled, clang-tidy probably won't work")
    endif()

    # Check everything
    add_custom_command(TARGET clang-tidy COMMAND ${clang-tidy} -enable-check-profile -p=${CMAKE_BINARY_DIR}/compile_commands.json -header-filter=sources/ -config= ${ALL_SOURCES})

    # Per-file check
    #foreach(SOURCE_FILE ${ALL_SOURCES})
    #    add_custom_command(TARGET clang-tidy COMMAND clang-tidy -enable-check-profile -p=${CMAKE_BINARY_DIR}/compile_commands.json -header-filter=Sources/ -config= ${SOURCE_FILE})
    #endforeach()
else()
    message(STATUS "No clang-tidy found")
endif()
