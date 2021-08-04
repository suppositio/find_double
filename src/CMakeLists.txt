project (${CMAKE_PROJECT_NAME})

add_library (File_compare STATIC File_compare.cpp File_compare.hpp)

add_library(Duplicate_finder STATIC Duplicate_finder.cpp Duplicate_finder.hpp)

target_link_libraries(Duplicate_finder PRIVATE
    File_compare
)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Duplicate_finder
)