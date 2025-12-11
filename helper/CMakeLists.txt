
cmake_minimum_required(VERSION 3.20)
project(helper VERSION 0.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
add_definitions(-std=gnu99 -O3 -Wall -fPIC)

option(ENABLE_DEBUG "compile for debug" OFF)
option(BUILD_STATIC "build static lib" OFF)
option(BUILD_SHARED "build shared lib" OFF)
option(STATIC_FIRST "static lib has higher priority" ON)

if (ENABLE_DEBUG)
	add_definitions(-DDBG -g)
endif()

file(GLOB_RECURSE C_FILES src/*.c)
set_source_files_properties(${C_FILES} PROPERTIES LANGUAGE C)

set(LIB_SRC ${C_FILES})
list(FILTER LIB_SRC EXCLUDE REGEX ".*/main\\.c$")

set(TARGET_LIB bsh_helper)
set(TARGET_LIBS "")
set(MAIN_LIB "")

set(TARGET_NAME helper)
add_executable(${TARGET_NAME} ${C_FILES})
target_include_directories(${TARGET_NAME} PRIVATE ${PROJECT_SOURCE_DIR})

if (BUILD_STATIC)
	add_library(${TARGET_LIB}_STATIC STATIC ${LIB_SRC})
	set_target_properties(${TARGET_LIB}_STATIC PROPERTIES OUTPUT_NAME ${TARGET_LIB})

	target_include_directories(${TARGET_LIB}_STATIC PUBLIC ${PROJECT_SOURCE_DIR})

	list(APPEND ${TARGET_LIBS} ${TARGET_LIB}_STATIC)

	if (STATIC_FIRST OR NOT BUILD_SHARED)
		set(MAIN_LIB ${TARGET_LIB}_STATIC)
	endif()
endif()

if (BUILD_SHARED)
	add_library(${TARGET_LIB}_SHARED SHARED ${LIB_SRC})
	set_target_properties(${TARGET_LIB}_SHARED PROPERTIES OUTPUT_NAME ${TARGET_LIB})

	target_include_directories(${TARGET_LIB}_SHARED PUBLIC ${PROJECT_SOURCE_DIR})

	list(APPEND ${TARGET_LIBS} ${TARGET_LIB}_SHARED)

	if (NOT STATIC_FIRST OR NOT BUILD_STATIC)
		set(MAIN_LIB ${TARGET_LIB}_SHARED)
	endif()
endif()

if (MAIN_LIB)
	add_library(helper_lib INTERFACE)
	target_link_libraries(helper_lib INTERFACE ${MAIN_LIB})
endif()

install(TARGETS ${TARGET_NAME} ${TARGET_LIBS}
	RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)

add_custom_target(clean_all
	COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
	COMMAND ${CMAKE_COMMAND} -E echo "Cleaning... Done"
)

set_target_properties(${TARGET_NAME} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

