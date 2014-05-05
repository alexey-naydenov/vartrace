set (VARTRACE_SRC utility.cc messageparser.cc log_level.cc)

add_library (vartrace ${VARTRACE_SRC})
target_link_libraries (vartrace stdc++)