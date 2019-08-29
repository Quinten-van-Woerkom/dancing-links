SET(SOURCE_LIST
	sudoku.cpp
	dancing_links.cpp
)

add_executable(sudoku ${SOURCE_LIST} ${HEADER_LIST})
target_include_directories(sudoku PUBLIC ../include)
target_include_directories(sudoku PUBLIC ../extern)
target_compile_features(sudoku PUBLIC cxx_std_20)