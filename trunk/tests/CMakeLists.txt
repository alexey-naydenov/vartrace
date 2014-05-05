include_directories (".")

set (test_srcs policy_test.cc creation_policy_test.cc utils_test.cc subtrace_test.cc
  selflog_test.cc containers_test.cc)
add_executable(vartrace_test ${test_srcs} vartrace_test.cc)
target_link_libraries(vartrace_test ${GTEST_LIB} vartrace pthread)
add_test(vartrace_test vartrace_test)

# different profile executables
add_executable(profile_char profile_char.cc)
target_link_libraries(profile_char vartrace)
add_test(profile_char profile_char)

add_executable(profile_int profile_int.cc)
target_link_libraries(profile_int vartrace)
add_test(profile_int profile_int)

add_executable(profile_double profile_double.cc)
target_link_libraries(profile_double vartrace)
add_test(profile_double profile_double)

add_executable(profile_subtrace profile_subtrace.cc)
target_link_libraries(profile_subtrace vartrace)
add_test(profile_subtrace profile_subtrace)

add_executable(profile profile.cc)
target_link_libraries(profile vartrace)
add_test(profile profile)

# program that creates logs for testing vartools
add_executable(generator generator.cc)
target_link_libraries(generator vartrace ${Boost_LIBRARIES} stdc++)