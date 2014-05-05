include_directories (".")

set (test_srcs policy_test.cc creation_policy_test.cc utils_test.cc subtrace_test.cc
  selflog_test.cc containers_test.cc)
add_executable(vartrace_test ${test_srcs} vartrace_test.cc)
target_link_libraries(vartrace_test ${GTEST_LIB} vartrace pthread)
add_test(vartrace_test vartrace_test)

add_executable(profile profile.cc)
target_link_libraries(profile vartrace)
add_test(profile profile)

# program that creates logs for testing vartools
add_executable(generator generator.cc)
target_link_libraries(generator vartrace ${Boost_LIBRARIES} stdc++)