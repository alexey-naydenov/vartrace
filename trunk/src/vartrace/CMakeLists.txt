set (VARTRACE_SRC vartrace.cpp messageparser.cpp)

add_library (vartrace ${VARTRACE_SRC})
target_link_libraries (vartrace stdc++)