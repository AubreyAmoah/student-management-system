# Find SQLite3
if(USE_SYSTEM_SQLITE)
    find_package(SQLite3 REQUIRED)
    if(NOT SQLite3_FOUND)
        message(FATAL_ERROR "System SQLite3 not found")
    endif()
else()
    # Include bundled SQLite3 or download it
    add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/sqlite3 ${CMAKE_BINARY_DIR}/sqlite3)
    set(SQLite3_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/third_party/sqlite3)
    set(SQLite3_LIBRARIES sqlite3)
endif()

# Add executable
add_executable(student_management_system
    main.cpp
    core/student.cpp
    core/database.cpp
    ui/console_ui.cpp
    utils/file_utils.cpp
)

# Include directories
target_include_directories(student_management_system PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${SQLite3_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(student_management_system PRIVATE
    ${SQLite3_LIBRARIES}
)

# Add compiler definitions based on platform
if(WIN32)
    target_compile_definitions(student_management_system PRIVATE
        OS_WINDOWS
        _CRT_SECURE_NO_WARNINGS
    )
elseif(APPLE)
    target_compile_definitions(student_management_system PRIVATE
        OS_MACOS
    )
elseif(UNIX)
    target_compile_definitions(student_management_system PRIVATE
        OS_LINUX
    )
endif()

# Set properties
set_target_properties(student_management_system PROPERTIES
    OUTPUT_NAME "student_management"
    WIN32_EXECUTABLE ON
)

# Add version info
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/version.h
)
target_include_directories(student_management_system PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
)

# Copy data files to build directory
add_custom_command(TARGET student_management_system POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/data
        ${CMAKE_BINARY_DIR}/bin/data
    COMMENT "Copying data files to build directory"
)

# Install targets
install(TARGETS student_management_system
    RUNTIME DESTINATION bin
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/core/
    DESTINATION include/core
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ui/
    DESTINATION include/ui
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/utils/
    DESTINATION include/utils
    FILES_MATCHING PATTERN "*.h"
)