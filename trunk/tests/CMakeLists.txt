add_executable (vartrace_test policy_test.cc creation_policy_test.cc vartrace_test.cc)
add_dependencies(vartrace_test gtest)
target_link_libraries (vartrace_test pthread)
target_link_libraries (vartrace_test vartrace)
add_test (vartrace_test vartrace_test)

# add_executable (vartrace_c_test vartrace_c_test.c)
# target_link_libraries (vartrace_c_test vartrace)
#add_test (vartrace_c_test vartrace_c_test)

# different profile executables
add_executable (profile_char profile_char.cc)
target_link_libraries (profile_char vartrace)
add_test (profile_char profile_char)

add_executable (profile_int profile_int.cc)
target_link_libraries (profile_int vartrace)
add_test (profile_int profile_int)

add_executable (profile_double profile_double.cc)
target_link_libraries (profile_double vartrace)
add_test (profile_double profile_double)

add_executable (profile_subtrace profile_subtrace.cc)
target_link_libraries (profile_subtrace vartrace)
add_test (profile_subtrace profile_subtrace)

add_custom_target (profile DEPENDS
  profile_int profile_char profile_double profile_subtrace)