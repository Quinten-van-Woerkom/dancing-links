add_executable(life life.cpp ${HEADER_LIST})
target_include_directories(life PUBLIC ../include)
target_compile_features(life PUBLIC cxx_std_17)