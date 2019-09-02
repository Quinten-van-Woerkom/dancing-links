cmake_minimum_required(VERSION 3.12)

project(Dancing_links
	VERSION 0.1
	DESCRIPTION "C++ implementation of Donald Knuth's dancing links algorithm for solving exact cover problems"
	LANGUAGES CXX)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	set(CMAKE_CXX_EXTENSIONS OFF)
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

if (MSVC)
	add_compile_options(/W4)
else()
	add_compile_options(-Wall -Wextra -Wpedantic)
endif()

enable_testing()

add_subdirectory(src)
add_subdirectory(tests)