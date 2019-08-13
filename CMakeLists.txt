cmake_minimum_required(VERSION 3.12)

project(Conway
	VERSION 0.1
	DESCRIPTION "C++ Hashlife implementation of Conway's Game of Life"
	LANGUAGES CXX)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	set(CMAKE_CXX_EXTENSIONS OFF)
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

add_subdirectory(src)