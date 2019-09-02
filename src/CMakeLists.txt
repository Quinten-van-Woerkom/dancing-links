SET(SOURCE_LIST
	main.cpp
	dancing_links.cpp
)

add_executable(dancing_links ${SOURCE_LIST} ${HEADER_LIST})
target_include_directories(dancing_links PUBLIC ../include)
target_include_directories(dancing_links PUBLIC ../extern)
target_compile_features(dancing_links PUBLIC cxx_std_20)