cmake_minimum_required(VERSION 3.25)

include(FetchContent)

FetchContent_Declare(
	dep_libyaml
	GIT_REPOSITORY https://github.com/yaml/libyaml.git
	GIT_TAG 0.2.5

	PATCH_COMMAND	${CMAKE_COMMAND}
					-D TARGET_FILE=<SOURCE_DIR>/CMakeLists.txt
					-P ${CMAKE_CURRENT_LIST_DIR}/fix_version.cmake
)

set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(dep_libyaml)
