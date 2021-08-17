project (File_compare_test)

find_package(GTest REQUIRED)

add_executable (${PROJECT_NAME} File_compare_test.cpp)

target_link_libraries (${PROJECT_NAME} PRIVATE
    GTest::GTest
    GTest::Main
    File_compare
)

include (GoogleTest)
gtest_discover_tests(${PROJECT_NAME})