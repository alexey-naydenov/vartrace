add_executable (vartrace_test policy_test.cc)
target_link_libraries (vartrace_test ${GTEST_BOTH_LIBRARIES} pthread)
target_link_libraries (vartrace_test vartrace)
add_test (vartrace_test vartrace_test)

add_executable (vartrace_c_test vartrace_c_test.c)
target_link_libraries (vartrace_c_test vartrace)
add_test (vartrace_c_test vartrace_c_test)

