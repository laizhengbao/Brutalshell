
cmake_minimum_required(VERSION 3.25)
project(BrutalShell VERSION 0.0.0 LANGUAGES C)

option(ENABLE_DEBUG "compile for debug" OFF)
option(BUILD_STATIC "build static lib" OFF)
option(BUILD_SHARED "build shared lib" ON)
option(STATIC_FIRST "static lib has higher priority" ON)
option(BUILD_TEST "build tests" OFF)

set(CMAKE_C_STANDARD 99)

# for libs
add_subdirectory(wrapper)
add_subdirectory(helper)

if (BUILD_TEST)
	include(CTest)
	enable_testing()
	add_subdirectory(test)
endif()
