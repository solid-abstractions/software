cmake_minimum_required(VERSION 3.0)
project(datastructures LANGUAGES CXX)

##############################################################################

add_compile_options(
    -Wall -Wextra -Werror
    -Wcast-align -Wcast-qual -Wold-style-cast
    -Wconversion -Wsign-conversion
    -Wshadow -Wstrict-overflow=3
)
include_directories(${PROJECT_SOURCE_DIR})

##############################################################################

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)
enable_testing()

add_executable(vector tests/vector_extent.cxx)
target_compile_options(vector PRIVATE -std=c++17)
target_include_directories(vector SYSTEM PRIVATE ${GTEST_INCLUDE_DIRS})
target_link_libraries(vector ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
add_test(NAME vector COMMAND vector)
