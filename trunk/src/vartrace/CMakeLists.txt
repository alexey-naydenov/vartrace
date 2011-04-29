set (VARTRACE_SRC vartrace.cc vartrace-internal.cc messageparser.cc)

add_library (vartrace ${VARTRACE_SRC})
target_link_libraries (vartrace stdc++)