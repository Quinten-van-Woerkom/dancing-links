SET(SOURCE_LIST
	life.cpp
	macrocell.cpp
)

add_executable(life ${SOURCE_LIST} ${HEADER_LIST})
target_include_directories(life PUBLIC ../include)
target_include_directories(life PUBLIC ../extern)
target_compile_features(life PUBLIC cxx_std_17)