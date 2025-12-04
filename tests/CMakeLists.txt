# Find testing framework (Google Test)
include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.13.0
)

FetchContent_MakeAvailable(googletest)

# Add test executable
add_executable(student_management_tests
    test_student.cpp
    test_database.cpp
)

target_include_directories(student_management_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/utils
    ${gtest_SOURCE_DIR}/include
    ${gmock_SOURCE_DIR}/include
)

target_link_libraries(student_management_tests PRIVATE
    GTest::gtest
    GTest::gtest_main
    ${SQLite3_LIBRARIES}
)

# Add tests
add_test(
    NAME StudentTests
    COMMAND student_management_tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

add_test(
    NAME DatabaseTests
    COMMAND student_management_tests --gtest_filter="DatabaseTest*"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Set test properties
set_tests_properties(StudentTests DatabaseTests PROPERTIES
    LABELS "unit"
    TIMEOUT 30
)