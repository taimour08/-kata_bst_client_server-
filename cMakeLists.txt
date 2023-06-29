enable_testing();
add_subdirectory(googletest)

add_executable(tests fizzbuzz_test.cpp)
target_link_libraries(tests gtest_main fizzbuzz)
add_test(NAME fizzbuzz_test COMMAND tests)