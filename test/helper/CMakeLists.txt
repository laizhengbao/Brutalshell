
cmake_minimum_required(VERSION 3.25)
project(helper_test VERSION 0.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)

set(TARGET_NAME ${PROJECT_NAME})

file(GLOB_RECURSE TEST_SRCS ./test_*.c)
add_executable(${TARGET_NAME} ./main.c ${TEST_SRCS})
target_include_directories(${TARGET_NAME} PRIVATE ${PROJECT_SOURCE_DIR})
target_link_libraries(${TARGET_NAME} PRIVATE helper_lib)
add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})
