cmake_minimum_required(VERSION 3.25)
project(wrapper VERSION 0.1 LANGUAGES C)

add_subdirectory(third_party)

option(ENABLE_DEBUG "compile for debug" OFF)
option(BUILD_STATIC "build static lib" OFF)
option(BUILD_SHARED "build shared lib" OFF)
option(STATIC_FIRST "static lib has higher priority" ON)

#set(CMAKE_INSTALL_PREFIX "/opt/tsl" CACHE STRING "Default installation directory")

set(CMAKE_C_STANDARD 99)
add_definitions(-std=gnu99 -O3 -Wall -fPIC)

if (ENABLE_DEBUG)
	add_definitions(-DDBG -g)
endif()

file(GLOB_RECURSE C_FILES src/*.c)

set(LIB_SRC	${C_FILES})
list(FILTER LIB_SRC EXCLUDE REGEX ".*/main\\.c$")

set_source_files_properties(${C_FILES} PROPERTIES LANGUAGE C)

set(TARGET_NAME bsh)
set(TARGET_LIB bsh_wrapper)
set(TARGET_LIBS "")
set(MAIN_LIB "")
add_executable(${TARGET_NAME} ${C_FILES})
target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(${TARGET_NAME} PRIVATE yaml)

if (BUILD_STATIC)
	add_library(${TARGET_LIB}_STATIC STATIC ${LIB_SRC})
	set_target_properties(${TARGET_LIB}_STATIC PROPERTIES OUTPUT_NAME ${TARGET_LIB})

	target_include_directories(${TARGET_LIB}_STATIC PUBLIC ${CMAKE_SOURCE_DIR})

	target_link_libraries(${TARGET_LIB}_STATIC PRIVATE yaml)

	list(APPEND ${TARGET_LIBS} ${TARGET_LIB}_STATIC)

	if (STATIC_FIRST OR NOT BUILD_SHARED)
		set(MAIN_LIB ${TARGET_LIB}_STATIC)
	endif()
endif()

if (BUILD_SHARED)
	add_library(${TARGET_LIB}_SHARED SHARED ${LIB_SRC})
	set_target_properties(${TARGET_LIB}_SHARED PROPERTIES OUTPUT_NAME ${TARGET_LIB})

	target_include_directories(${TARGET_LIB}_SHARED PUBLIC ${CMAKE_SOURCE_DIR})

	target_link_libraries(${TARGET_LIB}_SHARED PRIVATE yaml)

	list(APPEND ${TARGET_LIBS} ${TARGET_LIB}_SHARED)

	if (NOT STATIC_FIRST OR NOT BUILD_STATIC)
		set(MAIN_LIB ${TARGET_LIB}_SHARED)
	endif()
endif()

if (MAIN_LIB)
	add_library(wrapper_lib ALIAS ${MAIN_LIB})
endif()

#install(TARGETS ${TARGET_NAME} ${TARGET_NAME}_static
install(TARGETS ${TARGET_NAME} ${TARGET_LIBS}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
	LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
	RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)

add_custom_target(clean_all
	COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
	COMMAND ${CMAKE_COMMAND} -E echo "Cleaning... Done"
)

#set_target_properties(${TARGET_NAME} ${TARGET_NAME}_static PROPERTIES
set_target_properties(${INSTALL_TARGETS} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

