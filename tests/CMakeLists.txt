add_executable(life_test life_test.cpp ${HEADER_LIST})
target_include_directories(life_test PUBLIC ../include)
target_compile_features(life_test PUBLIC cxx_std_17)
add_test(LifeTest life_test)