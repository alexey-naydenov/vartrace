set (VARTRACE_SRC vartrace.cpp vartrace_c.cpp messageparser.cpp)

add_library (vartrace ${VARTRACE_SRC})
target_link_libraries (vartrace stdc++)