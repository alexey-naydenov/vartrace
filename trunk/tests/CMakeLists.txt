


add_executable (vartrace_test vartrace_test.cpp)
target_link_libraries (vartrace_test gtest)
target_link_libraries (vartrace_test gmock)
target_link_libraries (vartrace_test vartrace)
add_test (vartrace_test vartrace_test)

add_executable (simplestack_test simplestack_test.cpp)
target_link_libraries (simplestack_test stdc++)
target_link_libraries (simplestack_test gtest)
target_link_libraries (simplestack_test gmock)
add_test (simplestack_test simplestack_test)

add_executable (vartrace_c_test vartrace_c_test.c)
target_link_libraries (vartrace_c_test vartrace)
add_test (vartrace_c_test vartrace_c_test)

