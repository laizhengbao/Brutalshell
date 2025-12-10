
cmake_minimum_required(VERSION 3.20)
project(helper VERSION 0.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
add_definitions(-std=gnu99 -O3 -Wall -fPIC)

option(ENABLE_DEBUG "compile for debug" OFF)
if (ENABLE_DEBUG)
	add_definitions(-DDBG -g)
endif()

file(GLOB_RECURSE C_FILES src/*.c)
set_source_files_properties(${C_FILES} PROPERTIES LANGUAGE C)

set(LIB_SRC ${C_FILES})
list(FILTER LIB_SRC EXCLUDE REGEX ".*/main\\.c$")

set(TARGET_NAME helper)
add_executable(${TARGET_NAME} ${C_FILES})
target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_SOURCE_DIR})

install(TARGETS ${TARGET_NAME}
	RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)

option(ENABLE_TEST "build tests" ON)
if (ENABLE_TEST)
	include(CTest)
	enable_testing()

	file(GLOB_RECURSE TEST_SRCS test/test_*.c)
	add_executable(${TARGET_NAME}_test test/main.c ${TEST_SRCS} ${LIB_SRC})
	target_include_directories(${TARGET_NAME}_test PRIVATE ${CMAKE_SOURCE_DIR})
	add_test(NAME ${TARGET_NAME}_test COMMAND ${TARGET_NAME}_test)
endif()

add_custom_target(clean_all
	COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
	COMMAND ${CMAKE_COMMAND} -E echo "Cleaning... Done"
)

set_target_properties(${TARGET_NAME} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

