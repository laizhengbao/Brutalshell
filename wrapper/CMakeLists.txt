cmake_minimum_required(VERSION 3.25)
project(wrapper VERSION 0.1 LANGUAGES C)

add_subdirectory(third_party)

option(ENABLE_DEBUG "compile for debug" OFF)

#set(CMAKE_INSTALL_PREFIX "/opt/tsl" CACHE STRING "Default installation directory")

set(CMAKE_C_STANDARD 99)
add_definitions(-std=gnu99 -O3 -Wall -fPIC)

if (ENABLE_DEBUG)
	add_definitions(-DDBG -g)
endif()

file(GLOB_RECURSE C_FILES src/*.c)

set_source_files_properties(${C_FILES} PROPERTIES LANGUAGE C)

set(TARGET_NAME bsh)
add_executable(${TARGET_NAME} ${C_FILES})
target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(${TARGET_NAME} PRIVATE yaml)

#install(TARGETS ${TARGET_NAME} ${TARGET_NAME}_static
install(TARGETS ${TARGET_NAME}
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

