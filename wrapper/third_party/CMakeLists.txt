
find_package(PkgConfig QUIET)

pkg_check_modules(YAML yaml-0.1 QUIET IMPORTED_TARGET)

if (YAML_FOUND)
	message(STATUS "Found system libyaml")
	add_library(yaml ALIAS PkgConfig::YAML)
else()
	add_subdirectory(libyaml)
endif()
